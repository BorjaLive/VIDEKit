#pragma once

#include "../../imgui/header/imgui.h"
#include "../../imgui/sfml/imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "../Workspace/Scene.h"

#include "WindowUI.h"
#include "LabelUI.h"
#include "ButtonUI.h"
#include "TreeView.h"

#include "../../Framework/VIDECLexer.h"

#include "../../Framework/Variable.h"

using SFMLRenderWindow = sf::RenderWindow;

namespace VIDEKit {
	class Window;
}

namespace VIDEGui {

	class GraphicsUI
	{
	private:
		struct TreeControl {
			bool dragging;
			TreeView *tree;
		};


		VIDECLexer *videcSettings;

	public:
		enum GUIStyle {
			GUIS_Dark,
			GUIS_Light,
			GUIS_Default
		};

		struct Control {
			bool overAnyWindow;
			int hoveredWindows;
			bool displayVariableModal;
			VIDEKit::Vector2D variableModalPosition;
			VIDEKit::Vector2D variableWorldPosition;
			VIDEKit::Variable *selectedVariable;
		};

		GraphicsUI::Control control;

		TreeView *variablesTreeReference;
		std::function<void(VIDEGui::TreeView*)> onCreateVariable;

		struct DetailsPanelInfo {
			std::string nodeName;
			std::string nodeType;
			std::string nodeValue;
			std::vector<std::string> nodeTypes;
			VIDEGui::TreeView::ChildItem* toolboxVariableSelected;

			DetailsPanelInfo() {
				nodeName.resize(30);
				nodeValue.resize(1024);
				toolboxVariableSelected = nullptr;
			}

			void clear() {
				nodeName.clear();
				nodeName.resize(30);
				nodeValue.clear();
				nodeValue.resize(1024);
				toolboxVariableSelected = nullptr;
			}

			void load(const std::string &loadNodeName, const std::string &value, const std::string &type ,std::vector<std::string> types) {

				clear();

				for (int i = 0; i < loadNodeName.size(); i++) {
					nodeName[i] = loadNodeName.at(i);
				}

				for (int i = 0; i < value.size(); i++) {
					nodeValue[i] = value.at(i);
				}

				nodeType = type;
				nodeTypes = types;
			}
		};

		DetailsPanelInfo detailsInfo;

		bool initialized;

	private:
		std::vector<WindowUI*> m_windows;
		VIDEKit::Window *m_windowBase;

		TreeControl treeController;


		
	public:
		GraphicsUI(class VIDECLexer* settings);
		~GraphicsUI();

		bool create(VIDEKit::Window *window,GraphicsUI::GUIStyle style);
		void poll(sf::Event e);
		void update(VIDEKit::Scene *scene, sf::Time deltaTime);

		void addWindow(WindowUI *window);

		std::function<void(TreeView*)> onItemDrag;
		std::function<void(TreeView*, const VIDEKit::Vector2D &worldPosition, const std::string &sectionName, const std::string &objectName)> onItemDrop;

		std::vector<VIDEGui::WindowUI*> getWindows();

		bool displayMenu();

		void displayDetailsForVarType(const std::string &varTypename);

	private:
		void initializeLayout();
	};

}