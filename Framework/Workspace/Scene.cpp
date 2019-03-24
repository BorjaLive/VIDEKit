#include "Scene.h"

//FRAMEWORK COMPONENTS
#include "../Window.h"
#include "Camera.h"

//EXTRA
#include "../../Extra/Vector2D.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "../GUI/LabelUI.h"


namespace VIDEKit {


	Scene::Scene(Window *window) : m_window(window){
		//CREATE THE SCENE CAMERA
		m_camera = new VIDEKit::Camera(this);
		mouseController = MouseController();


		defaultFont.loadFromFile("fonts/Montserrat-Regular.ttf");
		zoomText = sf::Text("1:1", defaultFont);
		workspaceText = sf::Text("Workspace", defaultFont);

		zoomText.setCharacterSize(30);
		zoomText.setFillColor((sf::Color)Color("#808080"));

		workspaceText.setCharacterSize(30);
		workspaceText.setFillColor((sf::Color)Color("#808080"));

		workspaceText.setOrigin(workspaceText.getLocalBounds().width/2.0f, 0);
	}


	Scene::~Scene(){}

	/*
		SCENE METHODS
	*/
	void Scene::setup() {
		/*
			FIRED WHEN SCENE CREATED
		*/
		timer.restart();
	}

	void Scene::render() {
		/*
			FIRED EACH FRAME
		*/

		this->renderGrid(10000, 50);

		for (int i = 0; i < m_nodes.size(); i++) {

			std::vector<Connection*> &conns = m_nodes[i]->getProperties().connections;

			for (int j = 0; j < conns.size(); j++) {
				conns[j]->render(m_window->texture());
			}
		}

		for (int i = 0; i < m_nodes.size(); i++) {
			m_nodes[i]->render(m_window->texture());
		}

		
		this->renderInfo();
	}

	VIDEKit::Node* Scene::createNode(const std::string & nodeName, const VIDEKit::Vector2D &position, std::vector<VIDEKit::Connection::ConnectionInfo> inputInfos, std::vector<VIDEKit::Connection::ConnectionInfo> outputInfos)
	{
		VIDEKit::Node *createdNode = new VIDEKit::Node(nodeName);
		
		createdNode->getTransform().position = position;

		createdNode->load(inputInfos, outputInfos);

		m_nodes.push_back(createdNode);

		return m_nodes.back();
	}

	//RENDER THE BACKGROUND SCENE
	void Scene::renderGrid(int size, float spacing)
	{
		float quadCount = size / spacing;

		
		for (int xy = (int)-quadCount/2; xy <= (int)quadCount/2; xy++) {

			VIDEKit::Line line1({ Vector2D(-size / 2.0f, xy * spacing), Vector2D(size / 2.0f, xy * spacing) });
			VIDEKit::Line line2({ Vector2D(xy * spacing, -size / 2.0f), Vector2D(xy * spacing, size / 2.0f) });

			line1.fillColor = (xy % 4 == 0) ? Color(0, 0, 0) : Color(60,60,60);
			line2.fillColor = (xy % 4 == 0) ? Color(0, 0, 0) : Color(60, 60, 60);
			
			line1.thickness = 1;
			line2.thickness = 1;

			line1.draw(m_window->texture());
			line2.draw(m_window->texture());

		}

	}

	void VIDEKit::Scene::renderInfo() {

		/*
		//CREATE A SCREEN SPACE VIEW FOR TEXT
		sf::View screenView = m_window->frame()->getDefaultView();
		//SET THE VIEWPORT DIMENSIONS
		screenView.setViewport(sf::FloatRect(0, 0, 1024.0f / (float)m_window->frame()->getSize().x, 768.0f / (float)m_window->frame()->getSize().y));
		// SET THE NEW VIEW FOR FUTURE RENDERING(text only)
		m_window->frame()->setView(screenView);

		//SET POSITION OF ZOOM INFO AT (TOP,LEFT)
		zoomText.setPosition(0, 0);

		//GET THE DECIMALS OF THE ZOOM
		float decimal = m_camera->getProperties().zoom - (int)m_camera->getProperties().zoom;
		//DEFAULT ZOOM "1:1"
		std::string num = "1";
		std::string den = "1";

		//IF IS NOT EXACT VALUE(INT) CONVERT INTO A FRACTION(ONLY 2 DECIMALS)
		if (decimal*10.0 > 0.9) {
			num = std::to_string(int(m_camera->getProperties().zoom * 10.0));
			den = "10";
		}
		else {
			//ELSE PASS ALL THE INT VALUE
			num = std::to_string(int(m_camera->getProperties().zoom));
		}

		//SET UPDATED FRACTION TO STRING
		zoomText.setString(num + ":" + den);

		//SET THE WORKSPACE POSITION AT THE CENTER OF THE SCREEN AT THE TOP
		workspaceText.setPosition(m_window->frame()->getSize().x / 2.0f, 0);

		//DRAW THE ZOOM TEXT
		m_window->frame()->draw(zoomText);
		//DRAW THE WORKSPACE TEXT
		m_window->frame()->draw(workspaceText);
		//SET THE LAST VIEW(WORLDSPACE)
		*/

		m_window->frame()->setView(*m_camera->getView());

		
	}


	/*
		WINDOW CALLBACK
	*/

	void Scene::mouseDown(double localX, double localY, double worldX, double worldY, int button) {
		
		if (button == sf::Mouse::Button::Left && !mouseController.isPopupOpened) {

			mouseController.selectionInfo.selectedNodeIndex = -1;
			mouseController.selectionInfo.selectedConnectionIndex = -1;
			mouseController.selectionInfo.connectionSelectionType = VIDEKit::Connection::SelectionType::ST_None;
			mouseController.selectionInfo.sceneSelectionObjectType = VIDEKit::SceneSelectionType::SST_None;

			//CHECK IF WE SELECT ANY NODE
			for (int i = 0; i < m_nodes.size(); i++) {

				if (m_nodes[i] != mouseController.selectedNodeReference) {
					m_nodes[i]->getProperties().selected = false;
					m_nodes[i]->getProperties().dragging = false;
				}

				if (m_nodes[i]->pointOver(mouseController.globalCoords)) {
					//NODE INDEX ON THE ARRAY
					mouseController.selectionInfo.selectedNodeIndex = i;
					//UNDEFINED INDEX
					mouseController.selectionInfo.selectedConnectionIndex = -1;
					//SELECTED A NODE...
					mouseController.selectionInfo.sceneSelectionObjectType = VIDEKit::SceneSelectionType::SST_Node;
					//UNDEFINED CONNECTION SELECTED INFORMATION
					mouseController.selectionInfo.connectionSelectionType = VIDEKit::Connection::SelectionType::ST_None;

					//SET LOCAL DRAG OFFSET WITHIN THE NODE
					mouseController.selectionInfo.startDragLocalOffset = m_nodes[i]->getTransform().position - mouseController.globalCoords;


					//WE HAVE SELECTED A NODE AT THIS POINT
					this->onNodeSelected(mouseController.selectionInfo.selectedNodeIndex);

					break;
				}
				else {

					//IF WE ARE NOT SELECTING THIS NODE CHECK IF SELECT ONE OF HIS CONNECTIONS

					std::vector<VIDEKit::Connection*> &conns = m_nodes[i]->getProperties().connections;

					for (int j = 0; j < conns.size(); j++) {
						VIDEKit::Connection::SelectionType type = conns[j]->pointOver(mouseController.globalCoords);

						if (type != VIDEKit::Connection::SelectionType::ST_None && !conns[j]->getInfo().linkageInfo.linked) {

							//SET THE SELECTED CONNECTION INFO
							mouseController.selectionInfo.connectionSelectionType = type;
							//SELECTED A CONNECTION...
							mouseController.selectionInfo.sceneSelectionObjectType = VIDEKit::SceneSelectionType::SST_Connection;
							//NODE INDEX ON THE ARRAY
							mouseController.selectionInfo.selectedNodeIndex = i;
							//CONNECTION INDEX OF THIS NODE
							mouseController.selectionInfo.selectedConnectionIndex = j;

							mouseController.selectionInfo.startDragLocalOffset = (type == VIDEKit::Connection::SelectionType::ST_FromHandler) ? conns[j]->getProperties().from - this->mouseController.globalCoords : (type == VIDEKit::Connection::SelectionType::ST_ToHandler) ? conns[j]->getProperties().to - this->mouseController.globalCoords : Vector2D();

							//AT THIS POINT WE HAVE SELECTED A CONNECTION
							this->onConnectionSelected(mouseController.selectionInfo.selectedNodeIndex, mouseController.selectionInfo.selectedConnectionIndex, mouseController.selectionInfo.connectionSelectionType);

							break;
						}
					}
				}
			}
		}
		else if (button == sf::Mouse::Button::Right) {

			//CHECK IF RIGHT CLICK OVER ANYTHING

			mouseController.selectionInfo.selectedNodeIndex = -1;
			mouseController.selectionInfo.selectedConnectionIndex = -1;
			mouseController.selectionInfo.connectionSelectionType = VIDEKit::Connection::SelectionType::ST_None;
			mouseController.selectionInfo.sceneSelectionObjectType = VIDEKit::SceneSelectionType::SST_None;
			mouseController.selectedNodeReference = nullptr;

			//CHECK IF WE SELECT ANY NODE
			for (int i = 0; i < m_nodes.size(); i++) {

				if (m_nodes[i] != mouseController.selectedNodeReference) {
					m_nodes[i]->getProperties().selected = false;
					m_nodes[i]->getProperties().dragging = false;
				}

				if (m_nodes[i]->pointOver(mouseController.globalCoords)) {
					mouseController.selectedNodeReference = m_nodes[i];
					break;
				}
				else {

					//IF WE ARE NOT SELECTING THIS NODE CHECK IF SELECT ONE OF HIS CONNECTIONS

					std::vector<VIDEKit::Connection*> &conns = m_nodes[i]->getProperties().connections;

					bool found = false;

					for (int j = 0; j < conns.size(); j++) {
						VIDEKit::Connection::SelectionType type = conns[j]->pointOver(mouseController.globalCoords);

						if (conns[j]->getInfo().linkageInfo.linked && type != VIDEKit::Connection::SelectionType::ST_None) {

							mouseController.selectedNodeReference = m_nodes[i];
							mouseController.selectionInfo.selectedConnectionIndex = j;
							//EXIT IF FOUND
							found = true;

							break;
						}
					}

					if (found)
						break;
				}
			}


			mouseController.openPopup = true;
		}


		this->processObjectInteractions();
	}

	void Scene::mouseUp(double localX, double localY, double worldX, double worldY, int button) {

		//FIRED WHEN USER RELEASE THE MOUSE BUTTON
		

		for (int i = 0; i < m_nodes.size(); i++) {
			if (m_nodes[i] != mouseController.selectedNodeReference || i != mouseController.selectionInfo.selectedNodeIndex) {
				m_nodes[i]->getProperties().selected = false;
				m_nodes[i]->getProperties().dragging = false;
			}
			else {
				if (i != mouseController.selectionInfo.selectedNodeIndex) {
					mouseController.selectedNodeReference = nullptr;
					m_nodes[i]->getProperties().selected = false;
					m_nodes[i]->getProperties().dragging = false;
					
					m_window->getGUI()->detailsInfo.clear();
				}
				else {
					m_nodes[i]->getProperties().selected = true;
					m_nodes[i]->getProperties().dragging = true;
				}
			}
		}


		if (mouseController.mouseDrag == Vector2D(0.0f, 0.0f) && !mouseController.isPopupOpened) {

			if (button == sf::Mouse::Button::Left) {
				//CLICK OVER ANYTHING

				if (mouseController.selectionInfo.selectedNodeIndex != -1) {
					m_nodes[mouseController.selectionInfo.selectedNodeIndex]->getProperties().selected = true;
					m_nodes[mouseController.selectionInfo.selectedNodeIndex]->getProperties().dragging = true;
					mouseController.selectedNodeReference = m_nodes[mouseController.selectionInfo.selectedNodeIndex];

					std::vector<std::string> nodeTypes;

					auto dataTypes = m_window->videc->getDataTypes();

					for (int i = 0; i < dataTypes.size(); i++) {
						if (dataTypes[i]->typeName.compare("function") != 0 &&
							dataTypes[i]->typeName.compare("operator") != 0 && 
							dataTypes[i]->typeName.compare("event") != 0 && 
							dataTypes[i]->typeName.compare("execution") != 0) {

							nodeTypes.push_back(dataTypes[i]->typeName);

						}
					}

					if (mouseController.selectedNodeReference->getProperties().type == VIDEKit::Node::NodeType::NT_Variable) {

						auto vars = m_window->getVariables();

						std::string nodeType;

						for (int i = 0; i < vars.size();i++) {
							if (vars[i]->getName().compare(mouseController.selectedNodeReference->getProperties().name.c_str()) == 0) {

								nodeType = vars[i]->getType();

								break;
							}
						}

						m_window->getGUI()->detailsInfo.load(mouseController.selectedNodeReference->getProperties().name, ((VIDEKit::Variable*)(mouseController.selectedNodeReference->extra))->getValue() , nodeType, nodeTypes);
					}
					else {
						m_window->getGUI()->detailsInfo.load(mouseController.selectedNodeReference->getProperties().name, "", "", {});
					}
				}
				else {
					mouseController.selectedNodeReference = nullptr;
				}
			}

			mouseController.selectionInfo.sceneSelectionObjectType = SST_None;
			mouseController.selectionInfo.connectionSelectionType = VIDEKit::Connection::SelectionType::ST_None;
			mouseController.selectionInfo.selectedConnectionIndex = -1;
		}
		//IF WE RELEASE THE LEFT MOUSE BUTTON
		else if (button == sf::Mouse::Button::Left && !mouseController.isPopupOpened) {

			//CHECK IF WE WERE DRAGING AN ITEM
			if (mouseController.selectionInfo.sceneSelectionObjectType == SST_Node) {
				//IF WE RELEASE A NODE AFTER DRAG
				
				if (m_nodes[this->mouseController.selectionInfo.selectedNodeIndex] != mouseController.selectedNodeReference) {
					m_nodes[this->mouseController.selectionInfo.selectedNodeIndex]->getProperties().dragging = false;
					mouseController.selectedNodeReference = nullptr;
					mouseController.selectionInfo.selectedNodeIndex = -1;
				}

				//CLEAR THE SELECTED DRAG INFO PROPERTIES(undefined)
				mouseController.selectionInfo.sceneSelectionObjectType = SST_None;
				mouseController.selectionInfo.connectionSelectionType = VIDEKit::Connection::SelectionType::ST_None;
				mouseController.selectionInfo.selectedConnectionIndex = -1;


			}
			else if (mouseController.selectionInfo.sceneSelectionObjectType == SST_Connection) {
				//IF WE RELEASE A CONNECTION AFTER DRAG

				//CHECK IF WE RELEASE THIS CONNECTION ON OTHER
				for (int i = 0; i < m_nodes.size(); i++) {

					if (i == mouseController.selectionInfo.selectedNodeIndex) { continue; }
					else if (mouseController.selectionInfo.selectedNodeIndex == -1) { break; }

					std::vector<VIDEKit::Connection*> &cons = m_nodes[i]->getProperties().connections;

					for (int j = 0; j < cons.size(); j++) {


						VIDEKit::Connection::SelectionType type = cons[j]->pointOver(mouseController.globalCoords);

						if (type != VIDEKit::Connection::SelectionType::ST_None && type != mouseController.selectionInfo.connectionSelectionType && type != VIDEKit::Connection::SelectionType::ST_Connection) {

							m_nodes[mouseController.selectionInfo.selectedNodeIndex]->getProperties().connections[mouseController.selectionInfo.selectedConnectionIndex]->link(cons[j]);

							this->onConnectionDrop(mouseController.selectionInfo.selectedNodeIndex, mouseController.selectionInfo.selectedConnectionIndex, type, i, j, (type == Connection::SelectionType::ST_FromHandler) ? Connection::SelectionType::ST_ToHandler : Connection::SelectionType::ST_FromHandler);
							

							mouseController.selectionInfo.selectedNodeIndex = -1;	//FLAG TO STORE THAT WE RELEASE THE ROPE ON THIS ROPE

							break;
						}
					}
				}


				mouseController.selectionInfo.sceneSelectionObjectType = SST_None;
				mouseController.selectionInfo.connectionSelectionType = VIDEKit::Connection::SelectionType::ST_None;
				mouseController.selectionInfo.selectedConnectionIndex = -1;
				mouseController.selectionInfo.selectedNodeIndex = -1;
			}

		}







		this->processObjectInteractions();
	}

	void Scene::mouseMove(double localX, double localY, double worldX, double worldY) {

		//IF LEFT MOUSE PRESSED UPDATE THE VIEW
		if(this->mouseController.LeftMouseDown && this->mouseController.selectionInfo.sceneSelectionObjectType == SST_None){
			this->m_camera->setPosition(this->mouseController.globalDragOffset - ((Vector2D((float)localX, (float)localY) - this->mouseController.screenDragOffset)*this->getCamera()->getProperties().zoom));
		}


		this->processObjectInteractions();
	}

	void Scene::keyDown(sf::Event keyEvent) {

		if (keyEvent.key.code == sf::Keyboard::Key::Delete && mouseController.selectedNodeReference != nullptr) {
			
			for (int i = 0; i < m_nodes.size(); i++) {
				auto &cons = m_nodes[i]->getProperties().connections;

				if (m_nodes[i] == mouseController.selectedNodeReference) {
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

			if (mouseController.selectedNodeReference->getProperties().type == VIDEKit::Node::NodeType::NT_Constant) {

				for (int i = 0; i < m_window->getVariables().size(); i++) {
					if (m_window->getVariables()[i]->getName().compare(((VIDEKit::Variable*)mouseController.selectedNodeReference->extra)->getName()) == 0) {

						m_window->getVariables().erase(m_window->getVariables().begin() + i);

						break;

					}
				}

			}
			
			m_nodes.erase(m_nodes.begin() + mouseController.selectionInfo.selectedNodeIndex);

			mouseController.selectionInfo.selectedNodeIndex = -1;
			mouseController.selectedNodeReference = nullptr;
		}
		else if (keyEvent.key.code == sf::Keyboard::Key::Delete && m_window->getGUI()->detailsInfo.toolboxVariableSelected != nullptr) {

			VIDEGui::TreeView::ChildItem *toolboxVariableSelected = m_window->getGUI()->detailsInfo.toolboxVariableSelected;

			auto &vars = m_window->getVariables();

			for (int i = 0; i < vars.size(); i++) {
				if (vars[i]->getName().compare(((VIDEGui::LabelUI*)toolboxVariableSelected->item)->getText()) == 0) {
					vars.erase(vars.begin() + i);
					break;
				}
			}

			if (toolboxVariableSelected->parent != nullptr) {

				auto &childs = toolboxVariableSelected->parent->getChilds();

				for (int i = 0; i < childs.size(); i++) {
					if (((VIDEGui::LabelUI*)childs[i].item)->getText().compare(((VIDEGui::LabelUI*)toolboxVariableSelected->item)->getText()) == 0) {
						childs.erase(childs.begin() + i);
						break;
					}
				}

				m_window->getGUI()->detailsInfo.toolboxVariableSelected = nullptr;
			}
		}
		else if (keyEvent.key.code == sf::Keyboard::F5) {
			system("cls");
			m_window->m_graphProcessor->runInThread(m_window->videc);
		}
		else if (keyEvent.key.code == sf::Keyboard::F9) {
			system("cls");
			m_window->m_graphProcessor->runInThread(m_window->videc, true);
		}
		else if (keyEvent.key.code == sf::Keyboard::F10) {
			m_window->m_graphProcessor->step();
		}

		this->processObjectInteractions();
	}

	void Scene::keyUp(sf::Keyboard::Key keyCode) {





		this->processObjectInteractions();
	}

	void Scene::scroll(double delta) {
		
		delta /= 10.0f;

		float zoom = m_camera->getProperties().zoom + delta;

		m_camera->getProperties().zoom = zoom;

		m_camera->update();

		this->processObjectInteractions();
	}

	void Scene::processObjectInteractions(){

		if (this->mouseController.selectionInfo.sceneSelectionObjectType != SST_None) {
			//WE HAVE SELECTED AT LEAST AN OBJECT

			if (this->mouseController.selectionInfo.sceneSelectionObjectType == SST_Node) {				
				//MOVE THE CURRENT SELECTED NODE TO THE GLOBAL MOUSE POSITION

				m_nodes[this->mouseController.selectionInfo.selectedNodeIndex]->getTransform().setPosition(this->mouseController.globalCoords + this->mouseController.selectionInfo.startDragLocalOffset);
				m_nodes[this->mouseController.selectionInfo.selectedNodeIndex]->getProperties().dragging = true;
			}
			else if(this->mouseController.selectionInfo.sceneSelectionObjectType == SST_Connection){
				//MOVE THE CONNECTION HANDLER TO THE GLOBAL MOUSE POSITION

				if (this->mouseController.selectionInfo.connectionSelectionType == VIDEKit::Connection::SelectionType::ST_FromHandler) {
					m_nodes[this->mouseController.selectionInfo.selectedNodeIndex]->getProperties().connections[this->mouseController.selectionInfo.selectedConnectionIndex]->getInfo().fromPoint = this->mouseController.globalCoords + this->mouseController.selectionInfo.startDragLocalOffset;
				}
				else if (this->mouseController.selectionInfo.connectionSelectionType == VIDEKit::Connection::SelectionType::ST_ToHandler) {
					m_nodes[this->mouseController.selectionInfo.selectedNodeIndex]->getProperties().connections[this->mouseController.selectionInfo.selectedConnectionIndex]->getInfo().toPoint = this->mouseController.globalCoords + this->mouseController.selectionInfo.startDragLocalOffset;
				}
			}
		}
	}

	void Scene::update() {
		//NODE INFO CHANGED
		for (int i = 0; i < m_nodes.size(); i++) {
			if (m_nodes[i]->getProperties().type == VIDEKit::Node::NodeType::NT_Variable) {
				//UPDATE CONNECTIONS OF THIS VARIABLE

				auto vars = m_window->getVariables();
				
				VIDEKit::Variable *typeVariable = nullptr;

				VIDECLexer::DataType *videcType = nullptr;

				for (auto v : vars) {
					if (v->getName().compare(m_nodes[i]->getProperties().name) == 0) {
						typeVariable = v;
						break;
					}
				}

				if (typeVariable != nullptr) {

					videcType = this->m_window->videc->getType(typeVariable->getType());


					auto &cons = m_nodes[i]->getProperties().connections;

					for (int j = 0; j < cons.size(); j++) {

						auto connectionInfo = cons[j]->getInfo();

						if (connectionInfo.type.typeName.compare("exec") != 0) {
							//IS A VALUED CONNECTION


							cons[j]->getInfo().type.typeName = typeVariable->getName();
							cons[j]->getInfo().type.typeColor = videcType->typeColor;
						}
					}
				}
			}
		}


	}

	/*
		OBJECTS INTERACTION
	*/
	void Scene::onNodeSelected(uint32_t selectedNodeIndex) {
		//FIRED WHEN A NODE IS SELECTED



	}
	void Scene::onNodeDrag(uint32_t selectedNodeIndex){
		//FIRED WHILE DRAGING A NODE



	}
	void Scene::onNodeDrop(uint32_t selectedNodeIndex) {
		//FIRED WHEN A NODE IS DROPED


	}


	void Scene::onConnectionSelected(uint32_t selectedNodeIndex, uint32_t selectedConnectionIndex, VIDEKit::Connection::SelectionType selectedNodeConnectionType) {
		//FIRED WHEN CONNECTION IS SELECTED



	}
	void Scene::onConnectionDrag(uint32_t selectedNodeIndex, uint32_t selectedConnectionIndex, VIDEKit::Connection::SelectionType selectedNodeConnectionType) {
		//FIRED WHILE DRAGING A CONNECTION




	}
	void Scene::onConnectionDrop(	uint32_t selectedNodeIndex, uint32_t selectedConnectionIndex, VIDEKit::Connection::SelectionType selectedNodeConnectionType,
									uint32_t dropNodeIndex, uint32_t dropConnectionIndex, VIDEKit::Connection::SelectionType dropNodeConnectionType) {
		//FIRED WHEN CONNECTION DROP ON OTHER CONNECTION



	}




	/*
		GETTERS
	*/
	Window *Scene::getWindow() {
		return m_window;
	}

	Camera *Scene::getCamera() {
		return m_camera;
	}
	std::vector<VIDEKit::Node*>& Scene::getNodes(){
		return m_nodes;
	}
	VIDEKit::Node * Scene::getNode(const std::string & nodeUUID)
	{
		for (int i = 0; i < m_nodes.size(); i++) {
			if (m_nodes[i]->blockID.compare(nodeUUID) == 0) {
				return m_nodes[i];
			}
		}

		return nullptr;
	}
}
