#include "GraphicsUI.h"

#include "../Window.h"

#include "../../Extra/Utils.h"

#include "../../Extra/Color.h"

#include "../VIDECLexer.h"

#include <boost/algorithm/string.hpp>

namespace VIDEGui {

	GraphicsUI::GraphicsUI(VIDECLexer* settings)
	{
		this->treeController.tree = nullptr;
		this->treeController.dragging = false;

		this->videcSettings = settings;
		
		this->control.displayVariableModal = false;
		this->control.variableModalPosition = VIDEKit::Vector2D();

		this->initialized = false;


		onItemDrag = [&](TreeView* tree) {
			this->treeController.dragging = true;
			this->treeController.tree = tree;
		};

		onItemDrop = [&](TreeView *tree, const VIDEKit::Vector2D &worldPosition, const std::string &sectionName, const std::string &objectName) {

			control.selectedVariable = nullptr;

			if (sectionName.compare("Variables") == 0) {
				//DROP A VARIABLE
				control.displayVariableModal = true;
				control.variableModalPosition = m_windowBase->getScene()->mouseController.screenCoords;
				control.variableWorldPosition = m_windowBase->getScene()->mouseController.globalCoords;
				
				std::vector<VIDEKit::Variable*> vars = m_windowBase->getVariables();

				for (int i = 0; i < vars.size(); i++) {
					if (vars[i]->getName().compare(objectName) == 0) {
						control.selectedVariable = vars[i];
						break;
					}
				}
			}
			else {
				VIDECLexer::BehaviorDefinition *def = this->videcSettings->get(sectionName, objectName);

				if (def != nullptr) {

					std::vector<VIDEKit::Connection::ConnectionInfo> inputInfos;
					std::vector<VIDEKit::Connection::ConnectionInfo> outputInfos;

					VIDEKit::Node::NodeType nodeType;

					//PARSE INPUTs

					auto lists = def->targetObject.lists;

					VIDEKit::Color nodeColor;

					for (int i = 0; i < lists.size(); i++) {
						if (lists[i].name.compare("inputs") == 0) {
							for (int j = 0; j < lists[i].fields.size(); j++) {
								//PROPERTY 0 is the variable definition
								VIDEKit::Connection::ConnectionInfo info;

								VIDECLexer::DataType *type = this->videcSettings->getType(lists[i].fields[j].properties[0].type);

								info.type.typeColor = type->typeColor;
								info.type.typeName = type->typeName;
								info.type.connectionName = lists[i].fields[j].properties[0].name;

								inputInfos.push_back(info);
							}
						}
						else if (lists[i].name.compare("outputs") == 0) {
							for (int j = 0; j < lists[i].fields.size(); j++) {
								//PROPERTY 0 is the variable definition
								VIDEKit::Connection::ConnectionInfo info;

								VIDECLexer::DataType *type = this->videcSettings->getType(lists[i].fields[j].properties[0].type);

								info.type.typeColor = type->typeColor;
								info.type.typeName = type->typeName;
								info.type.connectionName = lists[i].fields[j].properties[0].name;

								outputInfos.push_back(info);
							}
						}
						else if (lists[i].name.compare("properties") == 0) {

							for (int j = 0; j < lists[i].fields.size(); j++) {
								VIDECLexer::Property *p = lists[i].fields[j].get("type");

								if (p != nullptr) {
									//SPECIAL CASE FUNCTION ADD IO EXECUTION CONNECTIONS
									if (p->value.compare("function") == 0) {
										VIDEKit::Connection::ConnectionInfo info;

										VIDECLexer::DataType *type = this->videcSettings->getType("execution");

										info.type.typeColor = type->typeColor;
										info.type.typeName = "exec";
										info.type.connectionName = "exec";

										inputInfos.push_back(info);
										outputInfos.push_back(info);

										nodeType = VIDEKit::Node::NodeType::NT_Function;
									}
									else if (p->value.compare("event") == 0) {

										VIDEKit::Connection::ConnectionInfo info;

										VIDECLexer::DataType *type = this->videcSettings->getType("execution");
										info.type.typeColor = type->typeColor;
										info.type.typeName = "exec";
										info.type.connectionName = "exec";

										outputInfos.push_back(info);

										nodeType = VIDEKit::Node::NodeType::NT_Event;
									}
									else if (p->value.compare("operator") == 0) {
										nodeType = VIDEKit::Node::NodeType::NT_Operator;
									}

									VIDECLexer::DataType *type = this->videcSettings->getType(p->value);

									nodeColor = type->typeColor;
								}
							}
						}
					}


					auto nodeReference = m_windowBase->getScene()->createNode(objectName, worldPosition, inputInfos, outputInfos);

					nodeReference->getProperties().accent = nodeColor;
					nodeReference->sectionName = sectionName;
					nodeReference->getProperties().type = nodeType;
				}
			}
		};
	}


	GraphicsUI::~GraphicsUI()
	{
		//FREE MEMORY
		for (auto &wnd : m_windows) {
			delete wnd;
		}
	}

	bool GraphicsUI::create(VIDEKit::Window *window, GraphicsUI::GUIStyle style)
	{
		m_windowBase = window;

		//INITIALIZE IMGUI
		ImGui::SFML::Init(*m_windowBase->frame());

		//ENABLE DOCK FOR LAYOUT MANIPULATION
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.IniFilename = NULL;
		

		ImGui::LoadIniSettingsFromDisk("imgui.ini");

		ImGuiStyle &imstyle = ImGui::GetStyle();

		imstyle.ChildRounding = 0.0f;
		imstyle.TabRounding = 0.0f;
		imstyle.FrameRounding = 0.0f;
		imstyle.WindowRounding = 0.0f;
		imstyle.GrabRounding = 5.0f;


		//SET THE GUI DEFAULT FONT
		auto &IO = ImGui::GetIO();
		IO.Fonts->Clear();
		IO.Fonts->AddFontFromFileTTF("fonts/Montserrat-Regular.ttf", 18.f);
		ImGui::SFML::UpdateFontTexture();

		//SET STYLE
		switch (style)
		{
		case VIDEGui::GraphicsUI::GUIS_Dark:
			ImGui::StyleColorsDark();
			break;
		case VIDEGui::GraphicsUI::GUIS_Light:
			ImGui::StyleColorsLight();
			break;
		case VIDEGui::GraphicsUI::GUIS_Default:
			break;
			ImGui::StyleColorsClassic();
		default:
			break;
		}

		ImGui::GetStyle().FrameBorderSize = 1.0f;


		//ON CREATE VARIABLE


		onCreateVariable =[&](TreeView* treeView) {

			std::vector<VIDEKit::Variable*> vars = m_windowBase->getVariables();

			int greatestUnnamed = 0;

			for (auto var : vars) {
				if (var->getName().find("var_") != std::string::npos) {
					greatestUnnamed = fmaxl(atoi(var->getName().substr(4).c_str()), greatestUnnamed);
				}
			}

			greatestUnnamed++;

			VIDEGui::TreeView::ChildItem *newVariable = new VIDEGui::TreeView::ChildItem();
			newVariable->draggable = true;
			newVariable->dragTagName = "var_" + std::to_string(greatestUnnamed);
			newVariable->itemNameStr = "Label";

			VIDEGui::ItemUI::Callbacks cbs;

			cbs.onSelected = [&](void *item) {

				VIDEGui::LabelUI* label = ((VIDEGui::LabelUI*)((VIDEGui::TreeView::ChildItem*)item)->item);

				std::vector<VIDEKit::Variable*> sceneVariables = m_windowBase->getVariables();

				std::string varType = "";

				for (int i = 0; i < sceneVariables.size(); i++) {
					if (sceneVariables[i]->getName().compare(label->getText()) == 0) {

						varType = sceneVariables[i]->getType();

						break;
					}
				}

				std::vector<std::string> nodeTypes;

				auto dataTypes = m_windowBase->videc->getDataTypes();

				for (int i = 0; i < dataTypes.size(); i++) {
					if (dataTypes[i]->typeName.compare("function") != 0 &&
						dataTypes[i]->typeName.compare("operator") != 0 &&
						dataTypes[i]->typeName.compare("event") != 0 &&
						dataTypes[i]->typeName.compare("execution") != 0) {

						nodeTypes.push_back(dataTypes[i]->typeName);

					}
				}


				detailsInfo.load(label->getText(), "", varType, nodeTypes);


				detailsInfo.toolboxVariableSelected = (VIDEGui::TreeView::ChildItem*)item;

				if (m_windowBase->getScene()->mouseController.selectedNodeReference != nullptr) {
					m_windowBase->getScene()->mouseController.selectedNodeReference->getProperties().selected = false;
				}
				m_windowBase->getScene()->mouseController.selectedNodeReference = nullptr;
			};

			cbs.onUnselected = [&](void *item) {

				detailsInfo.toolboxVariableSelected = nullptr;

			};

			newVariable->item = new VIDEGui::LabelUI("Variables->var_" + std::to_string(greatestUnnamed), "var_" + std::to_string(greatestUnnamed), cbs, true);

			newVariable->item->extra = (void*)newVariable;

			m_windowBase->addVariable(newVariable);

			treeView->addItem(*newVariable);
		};

		//INITIALIZE THE LAYOUT
		initializeLayout();

		m_windowBase->m_viewportWindow = m_windows[0];

		return false;
	}

	void GraphicsUI::poll(sf::Event e){
		//PROCESS EVENTS
		ImGui::SFML::ProcessEvent(e);

		if (e.type == sf::Event::EventType::MouseButtonReleased && m_windowBase->m_viewportWindow->pointOver(m_windowBase->getScene()->mouseController.screenCoords)) {
			if (treeController.tree != nullptr && treeController.dragging) {
				treeController.dragging = false;

				boost::smatch matches;

				if (boost::regex_match(treeController.tree->getSelectedChild()->item->id(),matches, boost::regex(Regex_Identifier+"\-\>"+Regex_Identifier))) {

					std::string sectionName = std::string(matches[1].begin(), matches[1].end());
					std::string objectName = std::string(matches[3].begin(), matches[3].end());

					onItemDrop(treeController.tree, m_windowBase->getScene()->mouseController.globalCoords, sectionName, objectName);
				}

				treeController.tree->clearSelection();
			}
		}
		
	}

	void GraphicsUI::update(VIDEKit::Scene * scene, sf::Time deltaTime){
		//RENDER
		ImGui::SFML::Update(*scene->getWindow()->frame(), deltaTime);

		if (displayMenu()) {
			return;
		}


		control.hoveredWindows = 0;
		
		auto sz = (VIDEKit::Vector2D)(sf::Vector2f)scene->getWindow()->frame()->getSize();

		sz.y -= 25;

		ImGuiViewport viewport = *ImGui::GetMainViewport();

		viewport.Size = (sf::Vector2f)sz;
		viewport.Pos.y = 25;

		ImGui::DockSpaceOverViewport(&viewport);

		for (int i = 0; i < m_windows.size(); i++) {

			m_windows[i]->display();
			if (PointRect(scene->mouseController.screenCoords, (sf::Vector2f)m_windows[i]->getProperties().position, (sf::Vector2f)m_windows[i]->getProperties().size)) {
				control.hoveredWindows++;
			}
		}

		if (m_windowBase->getScene()->mouseController.openPopup) {
			ImGui::OpenPopup("RightClickContextMenu");
			m_windowBase->getScene()->mouseController.openPopup = false;
		}

		if (ImGui::BeginPopupContextWindow("RightClickContextMenu")) {

			VIDEKit::MouseController &mouseController = m_windowBase->getScene()->mouseController;

			mouseController.isPopupOpened = true;

			if (ImGui::MenuItem("Delete", NULL, false, (mouseController.selectedNodeReference != nullptr && mouseController.selectionInfo.selectedConnectionIndex == -1))) {
				
				auto &nodes = scene->getNodes();

				for (int i = 0; i < nodes.size(); i++) {
					auto &cons = nodes[i]->getProperties().connections;

					if (nodes[i] == mouseController.selectedNodeReference) {
						for (int j = 0; j < cons.size(); j++) {
							cons.erase(cons.begin() + j);
						}
					}
					else {
						for (int j = 0; j < cons.size(); j++) {
							if (cons[j]->getInfo().linkageInfo.linkage != nullptr) {
								if (cons[j]->getInfo().linkageInfo.linkage->getInfo().from == mouseController.selectedNodeReference || cons[j]->getInfo().linkageInfo.linkage->getInfo().to == mouseController.selectedNodeReference) {
									cons[j]->getInfo().linkageInfo.linked = false;
									cons[j]->getInfo().linkageInfo.linkage = nullptr;
								}
							}
						}
					}
				}

				nodes.erase(nodes.begin() + mouseController.selectionInfo.selectedNodeIndex);

				mouseController.selectionInfo.selectedNodeIndex = -1;
				mouseController.selectedNodeReference = nullptr;


				ImGui::CloseCurrentPopup();
			}
			
			if (ImGui::MenuItem("Break Link", NULL, false, (mouseController.selectedNodeReference != nullptr && mouseController.selectionInfo.selectedConnectionIndex != -1))) {
				VIDEKit::Connection *connection = mouseController.selectedNodeReference->getProperties().connections[mouseController.selectionInfo.selectedConnectionIndex];
				
				connection->getInfo().linkageInfo.linkage->getInfo().linkageInfo.linked = false;
				connection->getInfo().linkageInfo.linkage->getInfo().linkageInfo.linkage = nullptr;

				connection->getInfo().linkageInfo.linked = false;
				connection->getInfo().linkageInfo.linkage = nullptr;

				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Make Literal", NULL, false, (mouseController.selectedNodeReference == nullptr && mouseController.selectionInfo.selectedConnectionIndex == -1))) {
				
				auto constants = m_windowBase->getVariables();

				int greatestUnnamed = 0;

				for (int i = 0; i < constants.size(); i++) {
					if (constants[i]->getName().find("constant_") != std::string::npos) {
						greatestUnnamed++;
					}
				}
				
				VIDEKit::Variable *literalVariable = m_windowBase->addVariable(nullptr, "constant_" + std::to_string(greatestUnnamed));
				
				std::vector<VIDEKit::Connection::ConnectionInfo> outputInfo;
				
				VIDEKit::Connection::ConnectionInfo valueConnection;
				
				VIDECLexer::DataType *defType = videcSettings->getDataTypes()[0];
				
				valueConnection.type.connectionName = "value";
				valueConnection.type.extra = "";
				valueConnection.type.typeColor = defType->typeColor;
				valueConnection.type.typeName = defType->typeName;
				
				outputInfo.push_back(valueConnection);
				
				VIDEKit::Node* createdNode = m_windowBase->getScene()->createNode("constant_" + std::to_string(greatestUnnamed), mouseController.globalCoords, {}, outputInfo);
				
				createdNode->extra = (void*)literalVariable;
				createdNode->getProperties().type = VIDEKit::Node::NodeType::NT_Variable;

				literalVariable->isConstant = true;
			}

			ImGui::EndPopup();
		}
		else {
			VIDEKit::MouseController &mouseController = m_windowBase->getScene()->mouseController;

			mouseController.isPopupOpened = false;
		}

		//RENDER
		ImGui::SFML::Render(*scene->getWindow()->frame());

		if (!this->initialized) {
			this->initialized = true;
		}
	}

	void GraphicsUI::addWindow(WindowUI * window)
	{
		assert(window != nullptr);
		m_windows.push_back(window);
	}

	std::vector<VIDEGui::WindowUI*> GraphicsUI::getWindows()
	{
		return m_windows;
	}

	bool GraphicsUI::displayMenu()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New")){
					if (OpenMessageDialog("New Graph", "All unsaved changes will be losed...\nAre you sure?")) {
						m_windowBase->m_graphProcessor->newGraph();
					}
				}

				if (ImGui::MenuItem("Load")) {

					if (OpenMessageDialog("Load Graph File", "All unsaved changes will be losed...\nAre you sure?")) {
						std::string filepath = OpenFileBrowser();

						if (filepath.compare("invalid") != 0) {
							m_windowBase->m_graphProcessor->load(filepath);
						}
					}
				}

				if (ImGui::MenuItem("Save as...")) {
					std::string filepath = SaveFileDialog();

					if (filepath.compare("invalid") != 0) {

						if (!boost::ends_with(filepath, ".videg")) {
							filepath += ".videg";
						}
						m_windowBase->save(filepath);
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit")) {
					exit(0);
				}



				ImGui::EndMenu();
			}


			if (ImGui::BeginMenu("Program"))
			{
				if (ImGui::MenuItem("Run", "F5")) {
					system("cls");
					m_windowBase->m_graphProcessor->runInThread(m_windowBase->videc);
				}

				if (ImGui::BeginMenu("Debug")) {
					if (ImGui::MenuItem("Start", "F9")) {
						system("cls");
						m_windowBase->m_graphProcessor->runInThread(m_windowBase->videc, true);
					}

					if (ImGui::MenuItem("Next Step", "F10")) {
						m_windowBase->m_graphProcessor->step();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Layout"))
			{

				if (ImGui::MenuItem("Reset")) {

					for (auto wnd : m_windows) {
						delete wnd;
					}

					ImGui::DestroyContext(ImGui::GetCurrentContext());

					this->create(m_windowBase, GUIS_Dark);

					//CREATE ALL THE VARIABLES
					
					auto vars = m_windowBase->getVariables();

					m_windowBase->getVariables().clear();

					for (int i = 0; i < vars.size(); i++) {
						if (!vars[i]->isConstant) {
							m_windowBase->getGUI()->onCreateVariable(m_windowBase->getGUI()->variablesTreeReference);

							m_windowBase->getVariables().back()->setName(vars[i]->getName());
							m_windowBase->getVariables().back()->setValue(vars[i]->getValue());
							m_windowBase->getVariables().back()->setType(vars[i]->getType());

							if (m_windowBase->getVariables().back()->getUIReference() != nullptr) {
								VIDEGui::LabelUI *uiRef = (VIDEGui::LabelUI*)m_windowBase->getVariables().back()->getUIReference()->item;

								uiRef->setText(vars[i]->getName());
								uiRef->setId("Variables->" + vars[i]->getName());
							}
						}
						else {
							m_windowBase->addVariable(nullptr, vars[i]->getName());

							m_windowBase->getVariables().back()->setName(vars[i]->getName());
							m_windowBase->getVariables().back()->setValue(vars[i]->getValue());
							m_windowBase->getVariables().back()->setType(vars[i]->getType());
							m_windowBase->getVariables().back()->isConstant = vars[i]->isConstant;
						}
					}


					return true;
				}

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		return false;
	}

	void GraphicsUI::displayDetailsForVarType(const std::string & varTypename)
	{
		
		VIDEKit::Variable *var = nullptr;
		
		if(ImGui::InputText("Name##NODE_PROPERTIES_TEXT", &detailsInfo.nodeName[0], 29, ImGuiInputTextFlags_EnterReturnsTrue)){

			for (int i = 0; i < m_windowBase->getVariables().size(); i++) {
				if (m_windowBase->getVariables().at(i)->getUUID().compare(((VIDEKit::Variable*)m_windowBase->getScene()->mouseController.selectedNodeReference->extra)->getUUID().c_str()) == 0) {

					var = m_windowBase->getVariables().at(i);

					var->setName(detailsInfo.nodeName.c_str());

					if (var->getUIReference() != nullptr) {
						((VIDEGui::LabelUI*)(var->getUIReference()->item))->setText(detailsInfo.nodeName.c_str());

						var->getUIReference()->item->setId("Variables->" + std::string(detailsInfo.nodeName.c_str()));

					}
					break;
				}
			}

			m_windowBase->getScene()->update();

		}

		if (ImGui::BeginCombo("Type##NODE_PROPERTIES_TYPE", detailsInfo.nodeType.c_str())) {

			for (int i = 0; i < detailsInfo.nodeTypes.size(); i++) {
				if (ImGui::Selectable((detailsInfo.nodeTypes[i] + "##NODE_PROPERTIES_TYPE_" + detailsInfo.nodeTypes[i]).c_str())) {

					detailsInfo.nodeType = detailsInfo.nodeTypes[i];

					for (int j = 0; j < m_windowBase->getVariables().size(); j++) {
						//if (m_windowBase->getVariables().at(j)->getName().compare(m_windowBase->getScene()->mouseController.selectedNodeReference->getProperties().name.c_str()) == 0) {
						if (m_windowBase->getVariables().at(j)->getName().compare(detailsInfo.nodeName.c_str()) == 0) {

							var = m_windowBase->getVariables().at(j);

							var->setType(detailsInfo.nodeType);

							std::string defValue = m_windowBase->videc->getType(detailsInfo.nodeType)->defaultTypeValue;

							var->setValue(defValue);

							detailsInfo.nodeValue.clear();
							detailsInfo.nodeValue.resize(1024);

							for (int k = 0; k < defValue.length(); k++) {
								detailsInfo.nodeValue[k] = defValue[k];
							}

							break;
						}
					}


					auto &nodes = m_windowBase->getScene()->getNodes();

					for (int j = 0; j < nodes.size(); j++) {
						if (nodes[j]->extra == var) {
							nodes[j]->update();
						}
					}


					//UPDATE SCENE VARIABLES WITH CHANGED TYPE
					m_windowBase->getScene()->update();

				}
			}

			ImGui::EndCombo();
		}

		//ImGui::Text("Default Value"); ImGui::SameLine(0.0f, 5.0f);

		std::string beforeValue = detailsInfo.nodeValue;

		if (varTypename.compare("integer") == 0) {
			ImGui::InputText("Default##NODE_PROPERTIES_INT_DEF_VALUE", &detailsInfo.nodeValue[0], 1023, ImGuiInputTextFlags_CharsDecimal);
		}
		else if (varTypename.compare("decimal") == 0) {
			ImGui::InputText("Default##NODE_PROPERTIES_DEC_DEF_VALUE", &detailsInfo.nodeValue[0], 1023, ImGuiInputTextFlags_CharsDecimal);
		}
		else if (varTypename.compare("string") == 0) {
			ImGui::InputText("Default##NODE_PROPERTIES_TXT_DEF_VALUE", &detailsInfo.nodeValue[0], 1023, ImGuiInputTextFlags_None);
		}
		else if (varTypename.compare("boolean") == 0) {
			
			std::string values[] = {"True", "False"};
			
			if (ImGui::BeginCombo("Default##NODE_PROPERTIES_BOO_DEF_VALUE", &detailsInfo.nodeValue[0])) {
				for (int i = 0; i < 2; i++) {
					if (ImGui::Selectable((values[i]+ "##NODE_PROPERTIES_TYPE_BOOL").c_str())) {
						detailsInfo.nodeValue.clear();
						detailsInfo.nodeValue.resize(1024);
						for (int j = 0; j < values[i].size(); j++) {
							detailsInfo.nodeValue[j] = values[i][j];
						}
					}
				}

				ImGui::EndCombo();
			}
		}


		if (beforeValue.compare(detailsInfo.nodeValue) != 0) {

			for (int j = 0; j < m_windowBase->getVariables().size(); j++) {

				if (m_windowBase->getVariables().at(j)->getName().compare(detailsInfo.nodeName.c_str()) == 0) {

					var = m_windowBase->getVariables().at(j);

					var->setValue((detailsInfo.nodeValue).c_str());

					break;
				}
			}
		}
	}

	void GraphicsUI::initializeLayout() {
		
		m_windows.clear();


		//CREATE THE MAIN VIEWPORT WINDOW
		auto viewportWindow = new VIDEGui::WindowUI(2, [&](VIDEGui::WindowUI *window, const std::string &title) {

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());

			ImGui::Begin(title.c_str(), NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ((!window->control.moving) ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoMove));


			VIDEGui::TreeView * view = (VIDEGui::TreeView*)this->getWindows().at(1)->getItems().back();

			if (ImGui::IsWindowFocused() && view != nullptr) {
				std::vector<VIDEGui::TreeView::ChildItem> childs = view->getChilds();

				for (int i = 0; i < childs.size(); i++) {
					if (childs[i].item->getSystem().selected) {
						childs[i].item->getSystem().selected = false;
						detailsInfo.clear();
						break;
					}
				}
			}

			window->getProperties().position = ImGui::GetWindowPos();
			window->getProperties().size = ImGui::GetWindowSize();

			sf::Sprite sprite(m_windowBase->texture()->getTexture());

			sf::Vector2f sz = (sf::Vector2f)m_windowBase->texture()->getSize();

			sprite.setTextureRect(sf::IntRect(0, sz.y, sz.x, -sz.y));

			ImGui::Image(sprite);

			if (control.displayVariableModal) {

				ImGui::OpenPopup("Variable");

				control.displayVariableModal = false;

				m_windowBase->displayVariableModal = true;
			}


			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 0.0f));

			if (ImGui::BeginPopup("Variable", ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {

				ImGui::PushStyleColor(ImGuiCol_Button, sf::Color(VIDEKit::Color("#141414")));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, sf::Color(VIDEKit::Color("#202020")));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, sf::Color(VIDEKit::Color("#252525")));

				if (ImGui::Button("SET")) {
					ImGui::CloseCurrentPopup();
					m_windowBase->displayVariableModal = false;
					control.displayVariableModal = false;

					VIDEKit::Connection::ConnectionInfo inType, inExec, outExec;

					auto typeInfo = m_windowBase->videc->getType(control.selectedVariable->getType());

					inType.type.connectionName = "New Value";
					inType.type.typeName = typeInfo->typeName;
					inType.type.typeColor = typeInfo->typeColor;

					inExec.type.connectionName = "exec";
					inExec.type.typeName = "exec";
					inExec.type.typeColor = VIDEKit::Color("#CCCCCC");

					outExec.type.connectionName = "exec";
					outExec.type.typeName = "exec";
					outExec.type.typeColor = VIDEKit::Color("#CCCCCC");

					std::vector<VIDEKit::Connection::ConnectionInfo> inputInfos{ inExec,inType };
					std::vector<VIDEKit::Connection::ConnectionInfo> outputInfos{ outExec };

					auto newNode = m_windowBase->getScene()->createNode(control.selectedVariable->getName(), control.variableWorldPosition, inputInfos, outputInfos);

					newNode->getProperties().type = VIDEKit::Node::NodeType::NT_Variable;

					newNode->extra = (void*)control.selectedVariable;
				}

				ImGui::SameLine();

				if (ImGui::Button("GET")) {
					ImGui::CloseCurrentPopup();
					m_windowBase->displayVariableModal = false;
					control.displayVariableModal = false;

					VIDEKit::Connection::ConnectionInfo outType;

					auto typeInfo = m_windowBase->videc->getType(control.selectedVariable->getType());

					outType.type.connectionName = "value";
					outType.type.typeName = typeInfo->typeName;
					outType.type.typeColor = typeInfo->typeColor;

					std::vector<VIDEKit::Connection::ConnectionInfo> outputInfos{ outType };

					auto newNode = m_windowBase->getScene()->createNode(control.selectedVariable->getName(), control.variableWorldPosition, {}, outputInfos);

					newNode->getProperties().type = VIDEKit::Node::NodeType::NT_Variable;

					newNode->extra = (void*)control.selectedVariable;
				}

				ImGui::PopStyleColor(3);

				ImGui::EndPopup();
			}
			else {
				control.displayVariableModal = false;
				m_windowBase->displayVariableModal = false;
			}
			ImGui::PopStyleVar(2);

			ImGui::End();

			ImGui::PopStyleVar();

		});

		viewportWindow->create("Viewport");

		m_windows.push_back(viewportWindow);




		//CREATE THE BLOCKS CONTAINER(toolbox)
		VIDEGui::WindowUI *toolbox = new VIDEGui::WindowUI(2);

		if (toolbox->create("ToolBox")) {

			//CREATE ALL THE VIDEC ELEMENTS

			std::vector<VIDECLexer::Section*> sections = this->videcSettings->getSections();
			for (int i = 0; i < sections.size(); i++) {
				VIDEGui::TreeView *view = new VIDEGui::TreeView("TreeView_" + sections[i]->name, sections[i]->name, onItemDrag);

				for (int j = 0; j < sections[i]->objects.size(); j++) {
					VIDEGui::TreeView::ChildItem visibleObject;
					visibleObject.item = new VIDEGui::LabelUI(sections[i]->name + "->" + sections[i]->objects[j].name, sections[i]->objects[j].name, VIDEGui::ItemUI::Callbacks(), true);
					visibleObject.draggable = true;
					visibleObject.itemNameStr = "Label";
					visibleObject.dragTagName = sections[i]->objects[j].name;

					view->addItem(visibleObject);
				}

				toolbox->addItem(view);
			}
			
			variablesTreeReference = new VIDEGui::TreeView("TreeView_Variables", "Variables", onItemDrag);

			variablesTreeReference->setExtraButton(true, "+", onCreateVariable);

			toolbox->addItem(variablesTreeReference);

			m_windows.push_back(toolbox);
		}


		//CREATE THE DETAILS PANEL
		VIDEGui::WindowUI *details = new VIDEGui::WindowUI(4, [&](VIDEGui::WindowUI *window, const std::string &title) {

				ImGui::Begin(title.c_str(), NULL);
				if (m_windowBase->getScene()->mouseController.selectedNodeReference != nullptr) {

					auto nodeType = m_windowBase->getScene()->mouseController.selectedNodeReference->getProperties().type;


					ImGui::Text("Node Properties");

					ImGui::Separator();

					if (nodeType == VIDEKit::Node::NodeType::NT_Variable) {

						displayDetailsForVarType(((VIDEKit::Variable*)(m_windowBase->getScene()->mouseController.selectedNodeReference->extra))->getType());

					}
				}
				else if (detailsInfo.toolboxVariableSelected != nullptr) {

					VIDEGui::LabelUI* label = ((VIDEGui::LabelUI*)(detailsInfo.toolboxVariableSelected->item));

					std::vector<VIDEKit::Variable*> sceneVariables = m_windowBase->getVariables();

					VIDEKit::Variable *var = nullptr;

					//FIND THE VARIABLE INFO
					for (int i = 0; i < sceneVariables.size(); i++) {
						if (sceneVariables[i]->getName().compare(label->getText()) == 0) {

							var = sceneVariables[i];

							break;
						}
					}

					if (var != nullptr) {

						//IS VARIABLE

						std::string beforeDisplayName = detailsInfo.nodeName;

						ImGui::Text("Name"); ImGui::SameLine(0.0f, 5.0f);

						ImGui::InputText("##NODE_PROPERTIES_TEXT", &detailsInfo.nodeName[0], 29, ImGuiInputTextFlags_None);

						if (detailsInfo.nodeName.compare(beforeDisplayName) != 0) {

							var->setName(detailsInfo.nodeName.c_str());
							
							if (var->getUIReference() != nullptr) {

								((VIDEGui::LabelUI*)(var->getUIReference()->item))->setText(detailsInfo.nodeName.c_str());

								var->getUIReference()->item->setId("Variables->" + std::string(detailsInfo.nodeName.c_str()));

							}

							m_windowBase->getScene()->update();
						}

						ImGui::Text("Type"); ImGui::SameLine(0.0f, 5.0f);

						if (ImGui::BeginCombo("##NODE_PROPERTIES_TYPE", detailsInfo.nodeType.c_str())) {



							for (int i = 0; i < detailsInfo.nodeTypes.size(); i++) {
								if (ImGui::Selectable((detailsInfo.nodeTypes[i] + "##NODE_PROPERTIES_TYPE_" + detailsInfo.nodeTypes[i]).c_str())) {

									detailsInfo.nodeType = detailsInfo.nodeTypes[i];

									for (int j = 0; j < m_windowBase->getVariables().size(); j++) {
										//if (m_windowBase->getVariables().at(j)->getName().compare(m_windowBase->getScene()->mouseController.selectedNodeReference->getProperties().name.c_str()) == 0) {
										if (m_windowBase->getVariables().at(j)->getName().compare(detailsInfo.nodeName.c_str()) == 0) {

											var = m_windowBase->getVariables().at(j);

											var->setType(detailsInfo.nodeType);

											break;
										}
									}


									auto &nodes = m_windowBase->getScene()->getNodes();

									for (int j = 0; j < nodes.size(); j++) {
										if (nodes[j]->extra == var) {
											nodes[j]->update();
										}
									}


									//UPDATE SCENE VARIABLES WITH CHANGED TYPE
									m_windowBase->getScene()->update();

								}
							}

							ImGui::EndCombo();




						}
					}
				}

				ImGui::End();
		});

		if (details->create("Details")) {
			m_windows.push_back(details);
		}



	}

}