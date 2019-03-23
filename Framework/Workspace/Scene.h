#pragma once

//FRAMEWORK COMPONENTS
#include "../../Extra/Shape/Line.h"
#include "../../Extra/Shape/Rectangle.h"
#include "../../Extra/Shape/Polygon.h"
#include "Connection.h"
#include "Node.h"

//EXTRA
#include "../../Extra/Color.h"


namespace VIDEKit {

	enum SceneSelectionType {
		SST_Connection,
		SST_Node,
		SST_None
	};

	struct SceneSelectionInfo {
		int selectedNodeIndex;
		int selectedConnectionIndex;
		SceneSelectionType sceneSelectionObjectType;
		VIDEKit::Connection::SelectionType connectionSelectionType;
		Vector2D startDragLocalOffset;

		SceneSelectionInfo() {
			selectedNodeIndex = -1;
			selectedConnectionIndex = -1;
			sceneSelectionObjectType = SceneSelectionType::SST_None;
			connectionSelectionType = VIDEKit::Connection::SelectionType::ST_None;
			startDragLocalOffset = Vector2D();
		}
	};

	struct MouseController {
		Vector2D screenCoords;
		Vector2D globalCoords;

		bool LeftMouseDown;
		bool RightMouseDown;
		bool MiddleMouseDown;

		Vector2D screenDragOffset;
		Vector2D globalDragOffset;

		Vector2D mouseDrag;

		SceneSelectionInfo selectionInfo;

		bool openPopup;
		bool isPopupOpened;

		VIDEKit::Node *selectedNodeReference;

		void clear() {
			screenCoords = Vector2D();
			globalCoords = Vector2D();

			LeftMouseDown = false;
			RightMouseDown = false;
			MiddleMouseDown = false;

			screenDragOffset = Vector2D();
			globalDragOffset = Vector2D();

			selectionInfo = SceneSelectionInfo();

			openPopup = false;
			isPopupOpened = false;
		}
	};


	class Scene
	{
	private:
		class Camera *m_camera;		//VIEWPORT OF THE SCENE
		class Window * m_window;	//DISPLAY WINDOW


		sf::Clock timer;
		sf::Font defaultFont;
		sf::Text zoomText;
		sf::Text workspaceText;


		/*
		
			SCENE NODES
		
		*/
		std::vector<VIDEKit::Node*> m_nodes;


	public:
		Scene(class Window *window);		//CTOR
		~Scene();	//DTOR

		virtual void setup();	//SETUP THE SCENE
		virtual void render();	//RENDER THE SCENE

		VIDEKit::Node *createNode(const std::string &nodeName, const VIDEKit::Vector2D &position, std::vector<VIDEKit::Connection::ConnectionInfo> inputInfos, std::vector<VIDEKit::Connection::ConnectionInfo> outputInfos);

		/*
			CALLBACKS
		*/

		void mouseDown(double localX, double localY, double worldX, double worldY, int button);
		void mouseUp(double localX, double localY, double worldX, double worldY, int button);
		void mouseMove(double localX, double localY, double worldX, double worldY);

		void keyDown(sf::Event keyEvent);
		void keyUp(sf::Keyboard::Key keyCode);

		void scroll(double delta);

		void processObjectInteractions();

		void update();

		/*
			EVENTS
		*/
		
		void onNodeSelected(	uint32_t selectedNodeIndex);
		void onNodeDrag(		uint32_t selectedNodeIndex);
		void onNodeDrop(		uint32_t selectedNodeIndex);


		void onConnectionSelected(	uint32_t selectedNodeIndex, uint32_t selectedConnectionIndex,	VIDEKit::Connection::SelectionType selectedNodeConnectionType);
		void onConnectionDrag(		uint32_t selectedNodeIndex, uint32_t selectedConnectionIndex,	VIDEKit::Connection::SelectionType selectedNodeConnectionType);
		void onConnectionDrop(		uint32_t selectedNodeIndex, uint32_t selectedConnectionIndex,	VIDEKit::Connection::SelectionType selectedNodeConnectionType,
									uint32_t dropNodeIndex,		uint32_t dropConnectionIndex,		VIDEKit::Connection::SelectionType dropNodeConnectionType);



		//GETTERs

		class Window *getWindow();
		class Camera *getCamera();

		std::vector<VIDEKit::Node*> &getNodes();

		VIDEKit::Node *getNode(const std::string &nodeUUID);

	private:
		//GRID RENDERING
		void renderGrid(int, float);

		//INFO RENDERING
		void renderInfo();


	public:
		MouseController mouseController;
	};

}