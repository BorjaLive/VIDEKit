#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "GraphProcessing.h"

//VIDEKit COMPONENTS
#include "GUI/GraphicsUI.h"
#include "Variable.h"

//EXTRA
#include "../Extra/Color.h"

//SFML
#include <SFML/Graphics.hpp>

//INCLUDES
#include <string>

//VIDEC
#include "VIDECLexer.h"

using SFMLRenderWindow = sf::RenderWindow;
using SFMLRenderTexture = sf::RenderTexture;

namespace VIDEKit {


	struct WindowProperties {
		std::string title;
		int width;
		int height;
		Color backgroundColor;
		bool closed;
		uint32_t framerate;
	};

	class Window
	{
	private:
		SFMLRenderWindow *m_renderWindow;
		SFMLRenderTexture *m_renderTexture;

		WindowProperties m_properties;

		class Scene *m_mainScene;

		class VIDEGui::GraphicsUI *m_gui;

		std::vector<VIDEKit::Variable*> m_variables;

	public:
		Window(const std::string &, const int&, const int&, const uint32_t& = 30);
		~Window();


		void proc();

		VIDEKit::Variable * addVariable(VIDEGui::TreeView::ChildItem *guiReference, std::string name = "variable");

	public:
		VIDECLexer *videc;

		bool displayVariableModal;

		class VIDEGui::WindowUI *m_viewportWindow;

		GraphProcessing *m_graphProcessor;

		static std::string BasePath;

	public:
		/*
			GETTERs
		*/
		WindowProperties getProperties();

		SFMLRenderWindow *frame();
		SFMLRenderTexture *texture();

		VIDEKit::Scene *getScene();

		std::vector<VIDEKit::Variable*> &getVariables();

		VIDEKit::Variable *getVariable(const std::string &variableName);

		VIDEGui::GraphicsUI *getGUI();

		void load(const std::string &path);
		void save(const std::string &path);

		void loadPlugins();
	};

}

#endif	//__WINDOW_H_