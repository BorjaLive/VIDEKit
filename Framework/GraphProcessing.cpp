#include "VIDECLexer.h"

#include "GraphProcessing.h"

#include <boost/filesystem.hpp>

#include <boost/lexical_cast.hpp>

#include "Window.h"

#include "Variable.h"

namespace VIDEKit {

	GraphProcessing::GraphProcessing(std::vector<VIDEKit::Node*> *nodes, VIDEKit::Window *referenceWindow)
	{
		nodesListReference = nodes;
		window = referenceWindow;
		executionThread = nullptr;
		debugControl.stepNext = false;
		debugControl.currentBlockUUID = "";
		debugControl.debugStepCheckMS = 100;
	}


	GraphProcessing::~GraphProcessing()
	{
	}

	void GraphProcessing::clear()
	{
		if (debugControl.active) {
			std::cout << "Program debug terminated, press any key to continue..." << std::endl;
			getchar();
		}

		for (int i = 0; i < nodesListReference->size(); i++) {
			if (nodesListReference->at(i)->getProperties().type == VIDEKit::Node::NodeType::NT_Function) {
				std::string decl = "result_" + nodesListReference->at(i)->blockID;

				PyRun_SimpleString(("if '" + decl + "' in globals():\n\tdel " + decl + "").c_str());
			}
			else if (nodesListReference->at(i)->getProperties().type == VIDEKit::Node::NodeType::NT_Operator) {
				std::string decl = "operation_" + nodesListReference->at(i)->blockID;

				PyRun_SimpleString(("if '" + decl + "' in globals():\n\tdel " + decl + "").c_str());
			}

			nodesListReference->at(i)->getProperties().isDebugged = false;
		}
	}

	void GraphProcessing::load(const std::string & path)
	{
		nodesListReference->clear();

		boost::filesystem::current_path(Window::BasePath);

		std::ifstream inputFile = std::ifstream(path.c_str(), std::ios::binary);

		if (!inputFile.is_open()) {
			return;
		}

		//READ ALL VARIABLES

		window->getVariables().clear();
		window->getGUI()->variablesTreeReference->clear();

		int varsCount = 0;
		inputFile.read(reinterpret_cast<char*>(&varsCount), sizeof(varsCount));

		for (int i = 0; i < varsCount; i++) {

			int name_len = 0;
			std::string name;
			int value_len = 0;
			std::string value;
			int type_len = 0;
			std::string type;
			int uuid_len = 0;
			std::string uuid;
			bool isConstant = false;

			inputFile.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));
			name.resize(name_len);
			inputFile.read(&name[0], name_len);

			inputFile.read(reinterpret_cast<char*>(&value_len), sizeof(value_len));
			value.resize(value_len);
			inputFile.read(&value[0], value_len);

			inputFile.read(reinterpret_cast<char*>(&type_len), sizeof(type_len));
			type.resize(type_len);
			inputFile.read(&type[0], type_len);

			inputFile.read(reinterpret_cast<char*>(&uuid_len), sizeof(uuid_len));
			uuid.resize(uuid_len);
			inputFile.read(&uuid[0], uuid_len);

			inputFile.read(reinterpret_cast<char*>(&isConstant), sizeof(bool));

			if (!isConstant) {
				window->getGUI()->onCreateVariable(window->getGUI()->variablesTreeReference);
			}
			else {
				window->addVariable(nullptr, name);
			}

			window->getVariables().back()->setName(name);
			window->getVariables().back()->setValue(value);
			window->getVariables().back()->setType(type);
			window->getVariables().back()->setUUID(uuid);
			window->getVariables().back()->isConstant = isConstant;

			if (!isConstant) {

				VIDEGui::LabelUI *uiRef = (VIDEGui::LabelUI*)window->getVariables().back()->getUIReference()->item;

				uiRef->setText(name);
				uiRef->setId("Variables->" + name);
			}
		}


		//READ NODE INFO AND THEN EACH CONNECTION FOR THIS NODE

		std::vector<ConnectionSaveData> connectionsInfos;



		int nodeCount = 0;
		inputFile.read(reinterpret_cast<char*>(&nodeCount), sizeof(nodeCount));

		for (int i = 0; i < nodeCount;i++) {

			std::vector<ConnectionSaveData> currentConnections;

			readBlockSettings(inputFile, nullptr, &currentConnections);

			connectionsInfos.insert(connectionsInfos.end(), currentConnections.begin(), currentConnections.end());
		}

		linkConnections(connectionsInfos);

		inputFile.close();
	}

	void GraphProcessing::save(const std::string & path)
	{
		std::ofstream outputFile = std::ofstream(path.c_str(), std::ios::binary | std::ios::trunc);

		if (!outputFile.is_open()) {
			return;
		}

		auto vars = window->getVariables();
		int varsCout = vars.size();

		outputFile.write(reinterpret_cast<char*>(&varsCout), sizeof(varsCout));

		for (int i = 0; i < varsCout; i++) {
			int name_len = vars[i]->getName().size();
			int value_len = vars[i]->getValue().size();
			int type_len = vars[i]->getType().size();
			int uuid_len = vars[i]->getUUID().size();

			outputFile.write(reinterpret_cast<char*>(&name_len), sizeof(name_len));
			outputFile.write(&vars[i]->getName()[0], name_len);

			outputFile.write(reinterpret_cast<char*>(&value_len), sizeof(value_len));
			outputFile.write(&vars[i]->getValue()[0], value_len);

			outputFile.write(reinterpret_cast<char*>(&type_len), sizeof(type_len));
			outputFile.write(&vars[i]->getType()[0], type_len);

			outputFile.write(reinterpret_cast<char*>(&uuid_len), sizeof(uuid_len));
			outputFile.write(&vars[i]->getUUID()[0], uuid_len);

			outputFile.write(reinterpret_cast<char*>(&vars[i]->isConstant), sizeof(bool));
		}

		int count = nodesListReference->size();

		outputFile.write(reinterpret_cast<char*>(&count), sizeof(count));

		for (int i = 0; i < nodesListReference->size(); i++) {
			saveBlockSettings(outputFile, nodesListReference->at(i));
		}

		outputFile.close();
	}

	void GraphProcessing::newGraph()
	{
		nodesListReference->clear();
		window->getVariables().clear();
		window->getGUI()->variablesTreeReference->clear();
	}

	void GraphProcessing::recursive_block_execution(VIDEKit::Node * currentNode, VIDECLexer *videc, VIDECLexer::BehaviorDefinition *definition, DebugControl *debug)
	{
		if (currentNode != nullptr) {

			if (debug != nullptr) {
				if (debug->active) {
					while (!debug->stepNext) {
						boost::this_thread::sleep_for(boost::chrono::milliseconds(debug->debugStepCheckMS));
					}

					VIDEKit::Node *prevBlock = window->getScene()->getNode(debug->currentBlockUUID);

					if (prevBlock != nullptr) {
						prevBlock->getProperties().isDebugged = false;
					}

					debug->currentBlockUUID = currentNode->blockID;

					debug->stepNext = false;

					if (currentNode != nullptr) {
						currentNode->getProperties().isDebugged = true;
					}
				}
			}

			std::vector<VIDEKit::Connection*> cons = currentNode->getProperties().connections;
			//GET INPUT PARAMETERS

			VIDECLexer::List *inputsList = nullptr;
			VIDECLexer::List *outputsList = nullptr;

			std::vector<std::string> parameters;

			std::vector<std::string> outputParameters;

			for (int i = 0; i < cons.size(); i++) {

				if (currentNode->getProperties().type != VIDEKit::Node::NodeType::NT_Variable) {


					if (cons[i]->getInfo().connectionType == VIDEKit::Connection::ConnectionType::CT_Input && cons[i]->getInfo().type.typeName.compare("exec") != 0) {

						VIDECLexer::BehaviorDefinition *currentDef = videc->get(currentNode->sectionName, currentNode->getProperties().name);

						inputsList = currentDef->targetObject.getList("inputs");


						if (inputsList != nullptr) {

							auto inputs = inputsList->fields;

							for (int j = 0; j < inputs.size(); j++) {
								if (inputs[j].properties[0].name.compare(cons[i]->getInfo().type.connectionName.c_str()) == 0) {


									if (cons[i]->getInfo().linkageInfo.linked && cons[i]->getInfo().linkageInfo.linkage->getInfo().from != nullptr) {

										if (cons[i]->getInfo().linkageInfo.linkage->getInfo().from->getProperties().type == VIDEKit::Node::NodeType::NT_Variable) {

											VIDEKit::Variable* asVarInfo = (VIDEKit::Variable*)(cons[i]->getInfo().linkageInfo.linkage->getInfo().from->extra);

											parameters.push_back(asVarInfo->getValue());

										}
										else if (cons[i]->getInfo().linkageInfo.linkage->getInfo().from->getProperties().type == VIDEKit::Node::NodeType::NT_Operator) {

											//GET OPERATION VALUE

											VIDEKit::Node *outputNode = cons[i]->getInfo().linkageInfo.linkage->getInfo().from;


											//GET THE OPERATOR DEFINITION

											VIDECLexer::BehaviorDefinition *nextBlockDefinition = nullptr;

											nextBlockDefinition = videc->get(outputNode->sectionName, outputNode->getProperties().name.c_str());


											recursive_block_execution(outputNode, videc, nextBlockDefinition, debug);

											parameters.push_back("operation_" + outputNode->blockID + "['" + cons[i]->getInfo().linkageInfo.linkage->getInfo().type.connectionName + "']");

										}
										else {
											//GET VALUE FROM OUTPUT

											VIDEKit::Node *outputNode = cons[i]->getInfo().linkageInfo.linkage->getInfo().from;

											parameters.push_back("result_" + outputNode->blockID + "['" + cons[i]->getInfo().linkageInfo.linkage->getInfo().type.connectionName + "']");

										}
									}
									else {
										parameters.push_back(inputs[j].properties[0].value);

									}
									break;
								}
							}

						}
					}
					else if (cons[i]->getInfo().connectionType == VIDEKit::Connection::ConnectionType::CT_Output && cons[i]->getInfo().type.typeName.compare("exec") != 0) {
						//AT THIS POINT, THIS CONNECTION IS A OUTPUT

						VIDECLexer::BehaviorDefinition *currentDefinition = videc->get(currentNode->sectionName, currentNode->getProperties().name);

						outputsList = currentDefinition->targetObject.getList("outputs");

						if (outputsList != nullptr) {

							auto outputs = outputsList->fields;

							for (int j = 0; j < outputs.size(); j++) {
								outputParameters.push_back(outputs[j].properties[0].name);
							}
						}
					}
				}
				else {

					if (cons[i]->getInfo().connectionType == VIDEKit::Connection::ConnectionType::CT_Input && cons[i]->getInfo().type.typeName.compare("exec") != 0) {

						if (cons[i]->getInfo().linkageInfo.linked && cons[i]->getInfo().linkageInfo.linkage->getInfo().from != nullptr) {

							if (cons[i]->getInfo().linkageInfo.linkage->getInfo().from->getProperties().type == VIDEKit::Node::NodeType::NT_Variable) {

								VIDEKit::Variable* asVarInfo = (VIDEKit::Variable*)(cons[i]->getInfo().linkageInfo.linkage->getInfo().from->extra);

								((VIDEKit::Variable*)(currentNode->extra))->setValue(asVarInfo->getValue());
							}
							else if (cons[i]->getInfo().linkageInfo.linkage->getInfo().from->getProperties().type == VIDEKit::Node::NodeType::NT_Operator || cons[i]->getInfo().linkageInfo.linkage->getInfo().from->getProperties().type == VIDEKit::Node::NT_Function) {

								//GET OPERATION VALUE

								VIDEKit::Node *outputNode = cons[i]->getInfo().linkageInfo.linkage->getInfo().from;


								//GET THE OPERATOR DEFINITION
								
								VIDECLexer::BehaviorDefinition *nextBlockDefinition = videc->get(outputNode->sectionName, outputNode->getProperties().name.c_str());

								if (outputNode->getProperties().type == VIDEKit::Node::NodeType::NT_Operator) {

									recursive_block_execution(outputNode, videc, nextBlockDefinition, debug);

									parameters.push_back("operation_" + outputNode->blockID + "['" + cons[i]->getInfo().linkageInfo.linkage->getInfo().type.connectionName + "']");
								}
								else {
									parameters.push_back("result_" + outputNode->blockID + "['" + cons[i]->getInfo().linkageInfo.linkage->getInfo().type.connectionName + "']");
								}

								//SETTING A VALUE

								PyObject *mainModule = PyImport_AddModule("__main__");
								PyObject *var = PyObject_GetAttrString(mainModule, parameters[0].substr(0, parameters[0].find("[")).c_str());

								if (var != nullptr) {

									size_t startindex = parameters[0].find("'");

									size_t endindex = parameters[0].find("'", startindex + 1);

									std::string key = parameters[0].substr(startindex + 1, (endindex - startindex) - 1);

									PyObject *targetVal = PyDict_GetItemString(var, key.c_str());

									if (targetVal != nullptr) {

										std::string typeName = Py_TYPE(targetVal)->tp_name;

										std::string valueStr = "";

										if (typeName.compare("str") == 0) {
											valueStr = _PyUnicode_AsString(targetVal);
										}
										else if (typeName.compare("int") == 0) {
											int val = PyLong_AsLong(targetVal);

											valueStr = std::to_string(val);
										}
										else if (typeName.compare("float") == 0) {
											double val = PyLong_AsDouble(targetVal);

											valueStr = std::to_string(val);
										}
										else if (typeName.compare("bool") == 0) {
											bool val = PyLong_AsLong(targetVal);

											valueStr = val ? "true" : "false";
										}

										((VIDEKit::Variable*)(currentNode->extra))->setValue(valueStr);
									}
								}

							}
							else {
								//GET VALUE FROM OUTPUT

								VIDEKit::Node *outputNode = cons[i]->getInfo().linkageInfo.linkage->getInfo().from;

								parameters.push_back("result_" + outputNode->blockID + "['" + cons[i]->getInfo().linkageInfo.linkage->getInfo().type.connectionName + "']");

							}
						}

						break;
					}
				}
			}

			//EXECUTE THE BLOCK

			if (definition != nullptr && definition->fooName.compare("onBegin") != 0 && definition->fooName.compare("For") != 0 && definition->fooName.compare("If") != 0 && definition->fooName.compare("While") != 0) {

				if (currentNode->getProperties().type != VIDEKit::Node::NodeType::NT_Variable) {

					std::string executionParametersStr = "";

					for (int i = 0; i < parameters.size(); i++) {
						executionParametersStr += (parameters[i] + ((i < parameters.size() - 1) ? "," : ""));
					}

					if (outputParameters.size() == 0) {
						PyRun_SimpleString((currentNode->sectionName + "_" + currentNode->getProperties().name + "(" + executionParametersStr + ")").c_str());
					}
					else {
						if (currentNode->getProperties().type == VIDEKit::Node::NodeType::NT_Function) {
							PyRun_SimpleString(("result_" + currentNode->blockID + "=" + (currentNode->sectionName + "_" + currentNode->getProperties().name + "(" + executionParametersStr + ")")).c_str());
						}
						else if (currentNode->getProperties().type == VIDEKit::Node::NodeType::NT_Operator) {
							PyRun_SimpleString(("operation_" + currentNode->blockID + "=" + (currentNode->sectionName + "_" + currentNode->getProperties().name + "(" + executionParametersStr + ")")).c_str());
						}
					}
				}
			}
			else if (definition != nullptr && definition->fooName.compare("For") == 0) {

				int start = 0;
				int end = 0;


				try {
					start = boost::lexical_cast<int>(parameters[0].c_str());
				}
				catch (...) {
					PyObject *mainModule = PyImport_AddModule("__main__");
					PyObject *var = PyObject_GetAttrString(mainModule, parameters[0].substr(0, parameters[0].find("[")).c_str());

					if (var != nullptr) {

						size_t startindex = parameters[0].find("'");

						size_t endindex = parameters[0].find("'", startindex + 1);

						std::string key = parameters[0].substr(startindex + 1, (endindex - startindex) - 1);

						PyObject *targetVal = PyDict_GetItemString(var, key.c_str());

						if (targetVal != nullptr) {

							start = atoi(_PyUnicode_AsString(targetVal));

						}

					}
				}

				try {
					end = boost::lexical_cast<int>(parameters[1].c_str());
				}
				catch (...) {
					PyObject *mainModule = PyImport_AddModule("__main__");
					PyObject *var = PyObject_GetAttrString(mainModule, parameters[1].substr(0, parameters[1].find("[")).c_str());

					if (var != nullptr) {

						size_t startindex = parameters[1].find("'");

						size_t endindex = parameters[1].find("'", startindex + 1);

						std::string key = parameters[1].substr(startindex + 1, (endindex - startindex) - 1);

						PyObject *targetVal = PyDict_GetItemString(var, key.c_str());

						if (targetVal != nullptr) {

							end = atoi(_PyUnicode_AsString(targetVal));

						}

					}
				}


				//GET ITERATION LINK

				VIDEKit::Node *startIterationBlock = nullptr;

				for (int i = 0; i < cons.size(); i++) {
					if (cons[i]->getInfo().linkageInfo.linked && cons[i]->getInfo().type.connectionName.compare("iteration") == 0) {

						startIterationBlock = cons[i]->getInfo().linkageInfo.linkage->getInfo().to;

						break;
					}
				}

				if (startIterationBlock) {

					VIDECLexer::BehaviorDefinition *firstBlockDefinition = nullptr;

					firstBlockDefinition = videc->get(startIterationBlock->sectionName,startIterationBlock->getProperties().name);

					//EXECUTE THE LOOP

					int scalar = (start <= end) ? 1 : -1;

					for (int i = start; i < end; i += scalar) {

						PyRun_SimpleString(("result_" + currentNode->blockID + " = " + definition->pyFooName +"(" + std::to_string(i) + ",None)").c_str());

						recursive_block_execution(startIterationBlock, videc, firstBlockDefinition, debug);
					}

				}
			}
			else if (definition != nullptr && definition->fooName.compare("While") == 0) {

				bool flag = true;

				if (parameters[0].compare("True") == 0) {
					flag = true;
				}
				else if(parameters[0].compare("False") == 0){
					flag = false;
				}
				else {

					try {
						flag = boost::lexical_cast<int>(parameters[0].c_str());
					}
					catch (...) {
						PyObject *mainModule = PyImport_AddModule("__main__");
						PyObject *var = PyObject_GetAttrString(mainModule, parameters[0].substr(0, parameters[0].find("[")).c_str());

						if (var != nullptr) {

							size_t startindex = parameters[0].find("'");

							size_t endindex = parameters[0].find("'", startindex + 1);

							std::string key = parameters[0].substr(startindex + 1, (endindex - startindex) - 1);

							PyObject *targetVal = PyDict_GetItemString(var, key.c_str());

							if (targetVal != nullptr) {

								std::string typeName = Py_TYPE(targetVal)->tp_name;

								if (typeName.compare("bool") == 0) {
									int n = PyLong_AsLong(targetVal);
									flag = n;
								}
								else if (typeName.compare("str") == 0) {

									std::string value = _PyUnicode_AsString(targetVal);

									if (value.compare("true") == 0) {
										flag = true;
									}
									else{
										flag = false;
									}
								}
								else if(typeName.compare("int") == 0){
									flag = PyLong_AsLong(targetVal);
								}
								else if(typeName.compare("float") == 0){
									flag = PyFloat_AsDouble(targetVal);
								}
								else {
									flag = false;
								}
							}
						}
					}
				}
				
				


				//GET ITERATION LINK

				VIDEKit::Node *startIterationBlock = nullptr;
				VIDEKit::Node *conditionalBlock = nullptr;

				for (int i = 0; i < cons.size(); i++) {
					if (cons[i]->getInfo().linkageInfo.linked && cons[i]->getInfo().type.connectionName.compare("iteration") == 0) {

						startIterationBlock = cons[i]->getInfo().linkageInfo.linkage->getInfo().to;

						if (conditionalBlock != nullptr) {
							break;
						}
					}
					else if (cons[i]->getInfo().linkageInfo.linked && cons[i]->getInfo().type.connectionName.compare("condition") == 0) {

						conditionalBlock = cons[i]->getInfo().linkageInfo.linkage->getInfo().from;

						if (startIterationBlock != nullptr) {
							break;
						}

					}
				}

				if (startIterationBlock && conditionalBlock) {

					VIDECLexer::BehaviorDefinition *firstBlockDefinition = nullptr;
					VIDECLexer::BehaviorDefinition *conditionalBlockDefinition = nullptr;

					firstBlockDefinition = videc->get(startIterationBlock->sectionName, startIterationBlock->getProperties().name);
					conditionalBlockDefinition = videc->get(conditionalBlock->sectionName, conditionalBlock->getProperties().name);


					while (flag) {

						recursive_block_execution(startIterationBlock, videc, firstBlockDefinition, debug);

						//UPDATE CONDITION
						
						recursive_block_execution(conditionalBlock, videc, conditionalBlockDefinition, debug);

						//CHECK

						if (parameters[0].compare("True") == 0) {
							flag = true;
						}
						else if (parameters[0].compare("False") == 0) {
							flag = false;
						}
						else {

							try {
								flag = boost::lexical_cast<int>(parameters[0].c_str());
							}
							catch (...) {
								PyObject *mainModule = PyImport_AddModule("__main__");
								PyObject *var = PyObject_GetAttrString(mainModule, parameters[0].substr(0, parameters[0].find("[")).c_str());

								if (var != nullptr) {

									size_t startindex = parameters[0].find("'");

									size_t endindex = parameters[0].find("'", startindex + 1);

									std::string key = parameters[0].substr(startindex + 1, (endindex - startindex) - 1);

									PyObject *targetVal = PyDict_GetItemString(var, key.c_str());

									if (targetVal != nullptr) {

										std::string typeName = Py_TYPE(targetVal)->tp_name;

										if (typeName.compare("bool") == 0) {
											int n = PyLong_AsLong(targetVal);
											flag = n;
										}
										else if (typeName.compare("str") == 0) {

											std::string value = _PyUnicode_AsString(targetVal);

											if (value.compare("true") == 0) {
												flag = true;
											}
											else {
												flag = false;
											}
										}
										else if (typeName.compare("int") == 0) {
											flag = _PyLong_AsInt(targetVal);
										}
										else if (typeName.compare("float") == 0) {
											flag = PyFloat_AsDouble(targetVal);
										}
										else {
											flag = false;
										}
									}
								}
							}
						}
					}

				}

			}
			else if (definition != nullptr && definition->fooName.compare("If") == 0) {

				if (parameters.size() == 0 || (parameters[0].compare("True") == 0 || parameters[0].compare("False") == 0)) {

					if (parameters.size() == 0 || parameters[0].compare("False") == 0) {

						//FIND THE BLOCK CONNECTED TO ELSE


						for (int i = 0; i < cons.size(); i++) {
							if (cons[i]->getInfo().type.connectionName.compare("else") == 0) {

								VIDECLexer::BehaviorDefinition *nextDefinition = nullptr;

								VIDEKit::Node *nextNode = cons[i]->getInfo().linkageInfo.linkage->getInfo().to;

								nextDefinition = videc->get(nextNode->sectionName, nextNode->getProperties().name.c_str());

								if (nextNode != nullptr) {
									recursive_block_execution(nextNode, videc, nextDefinition, debug);
								}

								break;
							}
						}

						//STOP HERE TO NOT CALL THE DEFAULT NEXT EXECUTION BLOCK "exec"
						return;
					}

				}
				else {

					PyObject *mainModule = PyImport_AddModule("__main__");
					PyObject *var = PyObject_GetAttrString(mainModule, parameters[0].substr(0, parameters[0].find("[")).c_str());

					if (var != nullptr) {

						size_t startindex = parameters[0].find("'");

						size_t endindex = parameters[0].find("'", startindex + 1);

						std::string key = parameters[0].substr(startindex + 1, (endindex - startindex) - 1);

						PyObject *targetVal = PyDict_GetItemString(var, key.c_str());

						if (targetVal != nullptr) {

							int b = PyLong_AsLong(targetVal);

							if (b == 0) {

								//FIND THE BLOCK CONNECTED TO ELSE


								for (int i = 0; i < cons.size(); i++) {
									if (cons[i]->getInfo().type.connectionName.compare("else") == 0) {

										
										VIDECLexer::BehaviorDefinition *nextDefinition = nullptr;


										VIDEKit::Node *nextNode = cons[i]->getInfo().linkageInfo.linkage->getInfo().to;

										nextDefinition = videc->get(nextNode->sectionName, nextNode->getProperties().name.c_str());

										if (nextNode != nullptr) {
											recursive_block_execution(nextNode, videc, nextDefinition, debug);
										}

										break;
									}
								}


								//STOP HERE TO NOT CALL THE DEFAULT NEXT EXECUTION BLOCK "exec"
								return;
							}
						}
					}
				}
			}



			//CHECK THE NEXT BLOCK

			//CHECK IF EXECUTION CONNECTION IS LINKED
			for (int i = 0; i < cons.size(); i++) {
				if (cons[i]->getInfo().linkageInfo.linked && cons[i]->getInfo().type.typeName.compare("exec") == 0 && cons[i]->getInfo().connectionType == VIDEKit::Connection::ConnectionType::CT_Output) {

					
					VIDECLexer::BehaviorDefinition *nextDefinition = nullptr;

					VIDEKit::Node *nextNode = cons[i]->getInfo().linkageInfo.linkage->getInfo().to;

					nextDefinition = videc->get(nextNode->sectionName, nextNode->getProperties().name.c_str());

					if (nextNode != nullptr) {
						recursive_block_execution(nextNode, videc, nextDefinition, debug);
					}

					break;
				}
			}
		}
	}

	void GraphProcessing::saveBlockSettings(std::ofstream &outputFile, VIDEKit::Node * nodeReference) {
		BlockSaveData savedata;

		savedata.name = nodeReference->getProperties().name.c_str();
		savedata.uuid = nodeReference->blockID.c_str();
		savedata.sectionName = nodeReference->sectionName.c_str();
		savedata.type = nodeReference->getProperties().type;
		savedata.x = nodeReference->getTransform().position.x;
		savedata.y = nodeReference->getTransform().position.y;
		savedata.nextConnectionsDataCount = 0;

		switch (nodeReference->getProperties().type) {
		case VIDEKit::Node::NodeType::NT_Event:
			savedata.typestr = "event";
			break;
		case VIDEKit::Node::NodeType::NT_Function:
			savedata.typestr = "function";
			break;
		case VIDEKit::Node::NodeType::NT_Operator:
			savedata.typestr = "operator";
			break;
		case VIDEKit::Node::NodeType::NT_Variable:
			savedata.typestr = "variable";
			break;
		case VIDEKit::Node::NodeType::NT_Constant:
			savedata.typestr = "constant";
			break;
		case VIDEKit::Node::NodeType::NT_Undefined:
			savedata.typestr = "undefined";
			break;
		default:
			savedata.typestr = "";
			break;
		}

		std::vector<ConnectionSaveData> connectionSaveInfos;

		auto cons = nodeReference->getProperties().connections;

		for (int i = 0; i < cons.size(); i++) {
			ConnectionSaveData connectionData;

			if (cons[i]->getInfo().linkageInfo.linked) {
				if (cons[i]->getInfo().linkageInfo.selfType == VIDEKit::Connection::SelfLinkType::SLT_From) {
					connectionData.fromuuid = cons[i]->getInfo().from->blockID.c_str();
					connectionData.fromconnectionname = cons[i]->getInfo().type.connectionName.c_str();
					connectionData.touuid = cons[i]->getInfo().linkageInfo.linkage->getInfo().to->blockID.c_str();
					connectionData.toconnectionname = cons[i]->getInfo().linkageInfo.linkage->getInfo().type.connectionName.c_str();
				}
				else {
					connectionData.touuid = cons[i]->getInfo().to->blockID.c_str();
					connectionData.toconnectionname = cons[i]->getInfo().type.connectionName.c_str();
					connectionData.fromuuid = cons[i]->getInfo().linkageInfo.linkage->getInfo().from->blockID.c_str();
					connectionData.fromconnectionname = cons[i]->getInfo().linkageInfo.linkage->getInfo().type.connectionName.c_str();
				}
			}
			else {
				if (cons[i]->getInfo().linkageInfo.selfType == VIDEKit::Connection::SelfLinkType::SLT_From) {
					connectionData.fromuuid = cons[i]->getInfo().from->blockID.c_str();
					connectionData.fromconnectionname = cons[i]->getInfo().type.connectionName.c_str();
					connectionData.touuid = "undefined";
					connectionData.toconnectionname = "undefined";
				}
				else if (cons[i]->getInfo().linkageInfo.selfType == VIDEKit::Connection::SelfLinkType::SLT_To) {
					connectionData.touuid = cons[i]->getInfo().to->blockID.c_str();
					connectionData.toconnectionname = cons[i]->getInfo().type.connectionName.c_str();
					connectionData.fromuuid = "undefined";
					connectionData.fromconnectionname = "undefined";
				}
			}

			connectionData.linkageType = cons[i]->getInfo().linkageInfo.selfType;

			connectionSaveInfos.push_back(connectionData);

			savedata.nextConnectionsDataCount++;
		}


		//SAVE THE BLOCK INFO

		int name_len = savedata.name.size();
		int uuid_len = savedata.uuid.size();
		int section_len = savedata.sectionName.size();
		int typestr_len = savedata.typestr.size();

		outputFile.write(reinterpret_cast<char*>(&name_len), sizeof(name_len));	//SAVE THE NAME
		outputFile.write(&savedata.name[0], name_len);

		outputFile.write(reinterpret_cast<char*>(&uuid_len), sizeof(uuid_len));	//SAVE THE UUID
		outputFile.write(&savedata.uuid[0], uuid_len);

		outputFile.write(reinterpret_cast<char*>(&section_len), sizeof(section_len));	//SAVE THE BLOCK SECTION
		outputFile.write(&savedata.sectionName[0], section_len);

		outputFile.write(reinterpret_cast<char*>(&savedata.nextConnectionsDataCount), sizeof(savedata.nextConnectionsDataCount));	//SAVE THE CONNECTIONS COUNT

		outputFile.write(reinterpret_cast<char*>(&savedata.x), sizeof(savedata.x));	//SAVE THE X COORDS
		outputFile.write(reinterpret_cast<char*>(&savedata.y), sizeof(savedata.y));	//SAVE THE X COORDS

		outputFile.write(reinterpret_cast<char*>(&typestr_len), sizeof(typestr_len));
		outputFile.write(&savedata.typestr[0], typestr_len);

		outputFile.write(reinterpret_cast<char*>(&savedata.type), sizeof(savedata.type));	//SAVE THE CONNECTION TYPE

		//SAVE THE CONNECTIONS

		for (int i = 0; i < connectionSaveInfos.size(); i++) {

			int from_uuid_len = connectionSaveInfos[i].fromuuid.size();
			int from_paramname_len = connectionSaveInfos[i].fromconnectionname.size();
			int to_uuid_len = connectionSaveInfos[i].touuid.size();
			int to_paramname_len = connectionSaveInfos[i].toconnectionname.size();

			outputFile.write(reinterpret_cast<char*>(&from_uuid_len), sizeof(from_uuid_len));
			outputFile.write(&connectionSaveInfos[i].fromuuid[0], from_uuid_len);

			outputFile.write(reinterpret_cast<char*>(&from_paramname_len), sizeof(from_paramname_len));
			outputFile.write(&connectionSaveInfos[i].fromconnectionname[0], from_paramname_len);

			outputFile.write(reinterpret_cast<char*>(&to_uuid_len), sizeof(to_uuid_len));
			outputFile.write(&connectionSaveInfos[i].touuid[0], to_uuid_len);

			outputFile.write(reinterpret_cast<char*>(&to_paramname_len), sizeof(to_paramname_len));
			outputFile.write(&connectionSaveInfos[i].toconnectionname[0], to_paramname_len);

			outputFile.write(reinterpret_cast<char*>(&connectionSaveInfos[i].linkageType), sizeof(connectionSaveInfos[i].linkageType));
		}
	}

	bool GraphProcessing::readBlockSettings(std::ifstream &inputFile, VIDEKit::Node * outputNode, std::vector<ConnectionSaveData> *nodeConnections) {


		//READ A BLOCK FROM FILE POINTER

		BlockSaveData currentBlockData;

		int name_len = 0;
		int uuid_len = 0;
		int section_len = 0;
		int typestr_len = 0;

		inputFile.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));	//READ THE NAME
		currentBlockData.name.resize(name_len);
		inputFile.read(&currentBlockData.name[0], name_len);

		if (currentBlockData.name.size() <= 0) {
			return false;
		}

		inputFile.read(reinterpret_cast<char*>(&uuid_len), sizeof(uuid_len));	//READ THE UUID
		currentBlockData.uuid.resize(uuid_len);
		inputFile.read(&currentBlockData.uuid[0], uuid_len);

		inputFile.read(reinterpret_cast<char*>(&section_len), sizeof(section_len));	//READ THE SECTION
		currentBlockData.sectionName.resize(section_len);
		inputFile.read(&currentBlockData.sectionName[0], section_len);

		inputFile.read(reinterpret_cast<char*>(&currentBlockData.nextConnectionsDataCount), sizeof(currentBlockData.nextConnectionsDataCount));	//READ THE INCOMMING CONNECTIONS

		inputFile.read(reinterpret_cast<char*>(&currentBlockData.x), sizeof(currentBlockData.x));	//READ THE X COORD
		inputFile.read(reinterpret_cast<char*>(&currentBlockData.y), sizeof(currentBlockData.y));	//READ THE Y COORD

		inputFile.read(reinterpret_cast<char*>(&typestr_len), sizeof(typestr_len));
		currentBlockData.typestr.resize(typestr_len);
		inputFile.read(&currentBlockData.typestr[0], typestr_len);

		inputFile.read(reinterpret_cast<char*>(&currentBlockData.type), sizeof(currentBlockData.type));	//READ THE BLOCK TYPE

		std::vector<VIDEKit::Connection::ConnectionInfo> inputInfos;
		std::vector<VIDEKit::Connection::ConnectionInfo> outputInfos;

		//GET INPUTS AND OUTPUTS

		VIDECLexer::BehaviorDefinition* def = window->videc->get(currentBlockData.sectionName, currentBlockData.name);

		VIDECLexer::List *inputsList = nullptr;
		VIDECLexer::List *outputsList = nullptr;

		if (def != nullptr) {
			inputsList = def->targetObject.getList("inputs");
			outputsList = def->targetObject.getList("outputs");
		}

		if (inputsList != nullptr && outputsList != nullptr) {

			for (int i = 0; i < inputsList->fields.size(); i++) {

				VIDEKit::Connection::ConnectionInfo info;

				info.type.connectionName = inputsList->fields[i].properties[0].name;
				info.type.typeColor = window->videc->getType(inputsList->fields[i].properties[0].type)->typeColor;
				info.type.typeName = inputsList->fields[i].properties[0].type;

				inputInfos.push_back(info);
			}

			for (int i = 0; i < outputsList->fields.size(); i++) {
				VIDEKit::Connection::ConnectionInfo info;

				VIDECLexer::DataType *type = window->videc->getType(outputsList->fields[i].properties[0].type);

				if (type != nullptr) {

					info.type.connectionName = outputsList->fields[i].properties[0].name;
					info.type.typeColor = type->typeColor;
					info.type.typeName = outputsList->fields[i].properties[0].type;

					outputInfos.push_back(info);
				}
			}

		}

		VIDEKit::Variable *extra = window->getVariable(currentBlockData.name);

		switch (currentBlockData.type) {
		case VIDEKit::Node::NodeType::NT_Event:
			outputInfos.push_back(VIDEKit::Connection::ConnectionInfo());

			outputInfos.back().type.typeName = "exec";
			outputInfos.back().type.typeColor = Color("#CCCCCC");
			outputInfos.back().type.connectionName = "exec";

			break;
		case VIDEKit::Node::NodeType::NT_Function:
			outputInfos.push_back(VIDEKit::Connection::ConnectionInfo());

			outputInfos.back().type.typeName = "exec";
			outputInfos.back().type.typeColor = Color("#CCCCCC");
			outputInfos.back().type.connectionName = "exec";

			inputInfos.push_back(VIDEKit::Connection::ConnectionInfo());

			inputInfos.back().type.typeName = "exec";
			inputInfos.back().type.typeColor = Color("#CCCCCC");
			inputInfos.back().type.connectionName = "exec";
			break;
		case VIDEKit::Node::NodeType::NT_Variable:

			if (currentBlockData.nextConnectionsDataCount == 3) {
				outputInfos.push_back(VIDEKit::Connection::ConnectionInfo());

				outputInfos.back().type.typeName = "exec";
				outputInfos.back().type.typeColor = Color("#CCCCCC");
				outputInfos.back().type.connectionName = "exec";

				inputInfos.push_back(VIDEKit::Connection::ConnectionInfo());

				inputInfos.back().type.typeName = "exec";
				inputInfos.back().type.typeColor = Color("#CCCCCC");
				inputInfos.back().type.connectionName = "exec";

				inputInfos.push_back(VIDEKit::Connection::ConnectionInfo());

				auto type = window->videc->getType(extra->getType());

				if (type != nullptr) {
					inputInfos.back().type.typeName = extra->getType();
					inputInfos.back().type.typeColor = type->typeColor;
					inputInfos.back().type.connectionName = "value";
				}
			}
			else {
				outputInfos.push_back(VIDEKit::Connection::ConnectionInfo());

				auto type = window->videc->getType(extra->getType());

				if (type != nullptr) {
					outputInfos.back().type.typeName = extra->getType();
					outputInfos.back().type.typeColor = type->typeColor;
					outputInfos.back().type.connectionName = "value";
				}
			}

			break;
		default:
			break;
		}

		if (nodeConnections != nullptr) {

			nodeConnections->clear();

			for (int i = 0; i < currentBlockData.nextConnectionsDataCount; i++) {

				ConnectionSaveData connectionData;

				int from_uuid_len = 0;
				int from_paramname_len = 0;
				int to_uuid_len = 0;
				int to_paramname_len = 0;
				int datatype_len = 0;

				inputFile.read(reinterpret_cast<char*>(&from_uuid_len), sizeof(from_uuid_len));
				connectionData.fromuuid.resize(from_uuid_len);
				inputFile.read(&connectionData.fromuuid[0], from_uuid_len);

				inputFile.read(reinterpret_cast<char*>(&from_paramname_len), sizeof(from_paramname_len));
				connectionData.fromconnectionname.resize(from_paramname_len);
				inputFile.read(&connectionData.fromconnectionname[0], from_paramname_len);

				inputFile.read(reinterpret_cast<char*>(&to_uuid_len), sizeof(to_uuid_len));
				connectionData.touuid.resize(to_uuid_len);
				inputFile.read(&connectionData.touuid[0], to_uuid_len);

				inputFile.read(reinterpret_cast<char*>(&to_paramname_len), sizeof(to_paramname_len));
				connectionData.toconnectionname.resize(to_paramname_len);
				inputFile.read(&connectionData.toconnectionname[0], to_paramname_len);

				inputFile.read(reinterpret_cast<char*>(&connectionData.linkageType), sizeof(connectionData.linkageType));

				nodeConnections->push_back(connectionData);
			}

		}

		//CREATE THE NODE ON THE SCENE
		VIDEKit::Node *createdNode = window->getScene()->createNode(currentBlockData.name, VIDEKit::Vector2D(currentBlockData.x, currentBlockData.y), inputInfos, outputInfos);
		VIDECLexer::DataType *type = window->videc->getType(currentBlockData.typestr);

		if (type != nullptr) {
			createdNode->getProperties().accent = type->typeColor;
		}

		createdNode->blockID = currentBlockData.uuid;

		createdNode->getProperties().type = currentBlockData.type;
		createdNode->sectionName = currentBlockData.sectionName;
		createdNode->extra = (void*)extra;


		if (outputNode != nullptr)
			outputNode = createdNode;

		return true;
	}

	void GraphProcessing::linkConnections(std::vector<ConnectionSaveData> connectionsInfos) {

		for (int i = 0; i < connectionsInfos.size(); i++) {

			VIDEKit::Node *from = nullptr;
			VIDEKit::Node *to = nullptr;

			if (connectionsInfos[i].fromuuid.compare("undefined") != 0) {
				from = window->getScene()->getNode(connectionsInfos[i].fromuuid);
			}

			if (connectionsInfos[i].touuid.compare("undefined") != 0) {
				to = window->getScene()->getNode(connectionsInfos[i].touuid);
			}

			if (from != nullptr && to != nullptr) {

				VIDEKit::Connection *fromlink = from->getProperties().getConnection(VIDEKit::Connection::ConnectionType::CT_Output,connectionsInfos[i].fromconnectionname);
				VIDEKit::Connection *tolink = to->getProperties().getConnection(VIDEKit::Connection::ConnectionType::CT_Input,connectionsInfos[i].toconnectionname);

				if (fromlink != nullptr && tolink != nullptr) {
					fromlink->link(tolink);
				}
			}
		}
	}

	void GraphProcessing::__simpleExec(VIDECLexer *videc)
	{
		__internalThreadRun = true;
		//FIND THE ENTRY POINT(onBegin)

		VIDEKit::Node *entryPoint = nullptr;

		for (int i = 0; i < nodesListReference->size(); i++) {
			if (nodesListReference->at(i)->getProperties().name.compare("onBegin") == 0) {
				entryPoint = nodesListReference->at(i);
				break;
			}
		}

		if (entryPoint != nullptr) {
			recursive_block_execution(entryPoint, videc, nullptr, (debugControl.active) ? &debugControl : nullptr);
		}


		//TERMINATED -> DELETE VARIABLES

		clear();
		
		__internalThreadRun = false;

	}

	void GraphProcessing::runInThread(VIDECLexer * videc, bool debug)
	{
		debugControl.active = debug;
		debugControl.stepNext = false;
		debugControl.currentBlockUUID = "";

		if (!__internalThreadRun) {
			executionThread = new boost::thread(boost::bind(&GraphProcessing::__simpleExec, this, videc));
		}
		else {
			//PROGRAM ALREADY RUNNING
		}
	}

	bool GraphProcessing::hasEndedRun()
	{
		return __internalThreadRun;
	}

	boost::thread *GraphProcessing::thread() {
		return executionThread;
	}

	void GraphProcessing::step() {
		debugControl.stepNext = debugControl.active;
	}
}