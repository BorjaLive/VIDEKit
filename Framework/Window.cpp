#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "Window.h"

#include "../Extra/Utils.h"

//FRAMEWORK COMPONENTS
#include "../Framework/Workspace/Scene.h"
#include "../Framework/Workspace/Camera.h"
#include "GUI/GraphicsUI.h"

#include <thread>
#include <chrono>
#include <iostream>

#include "../imgui/header/imgui.h"
#include "../imgui/sfml/imgui-SFML.h"


namespace VIDEKit {

	std::string Window::BasePath = "";

	Window::Window(const std::string &title, const int &width, const int &height, const uint32_t &framerate){

		Window::BasePath = boost::filesystem::current_path().string();

		/*
			STORE THE WINDOW PROPERTIES
		*/
		m_properties.width = width;
		m_properties.height = height;
		m_properties.title = title;
		m_properties.backgroundColor = Color(20, 20, 20, 255);
		m_properties.closed = false;
		m_properties.framerate = framerate;

		/*
			ENABLE ANTIALIASING
		*/
		sf::ContextSettings settings;
		settings.antialiasingLevel = 12;

		/*
			CREATE THE SFML RENDER WINDOW
		*/
		m_renderWindow = new sf::RenderWindow(sf::VideoMode(1024, 768), title.c_str(), sf::Style::Default, settings);
		m_renderWindow->setFramerateLimit(framerate);
		sf::Image icon;
		icon.loadFromFile("videkit-logo.png");
		m_renderWindow->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
		/*
			LOAD VIDEC SETTINGS
		*/

		this->videc = new VIDECLexer();

		this->loadPlugins();

		/*
			CREATE THE GUI INSTANCE
		*/
		m_gui = new VIDEGui::GraphicsUI(this->videc);
		m_gui->create(this, VIDEGui::GraphicsUI::GUIStyle::GUIS_Dark);

		m_renderTexture = new sf::RenderTexture();

		/*
			CREATE THE MAIN SCENE
		*/
		m_mainScene = new VIDEKit::Scene(this);

		/*
			GRAPH PROCESSING CLASS
		*/

		m_graphProcessor = new VIDEKit::GraphProcessing(&m_mainScene->getNodes(), this);
	}


	Window::~Window()
	{

	}

	void Window::proc()
	{

		m_mainScene->setup();

		sf::Clock deltaClock;

		do {
			sf::Event windowEvent;

			m_renderTexture->create(m_renderWindow->getSize().x, m_renderWindow->getSize().y, m_renderWindow->getSettings());

			if (m_renderWindow->pollEvent(windowEvent)) {
				//PROCESS THE MAIN GUI SYSTEM
				m_gui->poll(windowEvent);

				sf::Vector2f viewSize = (sf::Vector2f)m_viewportWindow->getProperties().size;
				sf::Vector2f viewPosition = (sf::Vector2f)m_viewportWindow->getProperties().position;


				bool pointOverView = PointRect(m_mainScene->mouseController.screenCoords, (VIDEKit::Vector2D)viewPosition + VIDEKit::Vector2D(10, 30), (VIDEKit::Vector2D)viewSize - VIDEKit::Vector2D(20, 35));
				

				float frameHeight = ImGui::GetFrameHeightWithSpacing() - 5;
				float frameWidth = 0;

				switch (windowEvent.type)
				{
				case sf::Event::EventType::Closed:
					m_properties.closed = true;
					break;
				case sf::Event::EventType::Resized:
					break;
				case sf::Event::EventType::MouseButtonPressed:

					m_mainScene->mouseController.screenCoords = Vector2D(
						windowEvent.mouseButton.x,
						windowEvent.mouseButton.y
					);

					m_mainScene->mouseController.globalCoords = Vector2D(
						windowEvent.mouseButton.x,
						windowEvent.mouseButton.y
					);
					
					m_mainScene->mouseController.mouseDrag = m_mainScene->mouseController.screenCoords;

					m_mainScene->mouseController.globalCoords = m_renderWindow->mapPixelToCoords((sf::Vector2i)(sf::Vector2f)m_mainScene->mouseController.globalCoords);
					
					m_mainScene->mouseController.globalCoords += Vector2D(-m_viewportWindow->getProperties().position.x - frameWidth, -m_viewportWindow->getProperties().position.y - frameHeight) * m_mainScene->getCamera()->getProperties().zoom;


					if (windowEvent.mouseButton.button == 0) {
						m_mainScene->mouseController.LeftMouseDown = true;
						m_mainScene->mouseController.screenDragOffset = m_mainScene->mouseController.screenCoords;
						m_mainScene->mouseController.globalDragOffset = m_mainScene->getCamera()->getView()->getCenter();
					}
					else if (windowEvent.mouseButton.button == 1) {
						m_mainScene->mouseController.RightMouseDown = true;
					}
					else if (windowEvent.mouseButton.button == 2) {
						m_mainScene->mouseController.MiddleMouseDown = true;
					}

					if (pointOverView) {

						m_mainScene->mouseDown(
							windowEvent.mouseButton.x,
							windowEvent.mouseButton.y,
							m_mainScene->mouseController.globalCoords.x,
							m_mainScene->mouseController.globalCoords.y,
							windowEvent.mouseButton.button);

					}

					break;
				case sf::Event::EventType::MouseButtonReleased:

					m_mainScene->mouseController.screenCoords = Vector2D(
						windowEvent.mouseButton.x,
						windowEvent.mouseButton.y
					);

					m_mainScene->mouseController.globalCoords = Vector2D(
						windowEvent.mouseButton.x,
						windowEvent.mouseButton.y
					);

					m_mainScene->mouseController.mouseDrag = (Vector2D)m_mainScene->mouseController.screenCoords - m_mainScene->mouseController.mouseDrag;

					m_mainScene->mouseController.globalCoords = m_renderWindow->mapPixelToCoords((sf::Vector2i)(sf::Vector2f)m_mainScene->mouseController.globalCoords);
					
					m_mainScene->mouseController.globalCoords += Vector2D(-m_viewportWindow->getProperties().position.x - frameWidth, -m_viewportWindow->getProperties().position.y - frameHeight) * m_mainScene->getCamera()->getProperties().zoom;

					if (windowEvent.mouseButton.button == 0) {
						m_mainScene->mouseController.LeftMouseDown = false;
					}
					else if (windowEvent.mouseButton.button == 1) {
						m_mainScene->mouseController.RightMouseDown = false;
					}
					else if (windowEvent.mouseButton.button == 2) {
						m_mainScene->mouseController.MiddleMouseDown = false;
					}


					if (pointOverView) {
						m_mainScene->mouseUp(
							windowEvent.mouseButton.x,
							windowEvent.mouseButton.y,
							m_mainScene->mouseController.globalCoords.x,
							m_mainScene->mouseController.globalCoords.y,
							windowEvent.mouseButton.button);

					}
					break;
				case sf::Event::EventType::MouseMoved:

					m_mainScene->mouseController.screenCoords = Vector2D(
						windowEvent.mouseMove.x,
						windowEvent.mouseMove.y
					);

					m_mainScene->mouseController.globalCoords = Vector2D(
						windowEvent.mouseMove.x,
						windowEvent.mouseMove.y
					);

					m_mainScene->mouseController.globalCoords = m_renderWindow->mapPixelToCoords((sf::Vector2i)(sf::Vector2f)m_mainScene->mouseController.globalCoords);

					m_mainScene->mouseController.globalCoords += Vector2D(-m_viewportWindow->getProperties().position.x - frameWidth, -m_viewportWindow->getProperties().position.y - frameHeight) * m_mainScene->getCamera()->getProperties().zoom;

					if (pointOverView) {

						m_mainScene->mouseMove(
							windowEvent.mouseMove.x,
							windowEvent.mouseMove.y,
							m_mainScene->mouseController.globalCoords.x,
							m_mainScene->mouseController.globalCoords.y);

					}
					break;
				case sf::Event::EventType::KeyPressed:
					m_mainScene->keyDown(windowEvent);
					break;
				case sf::Event::EventType::KeyReleased:
					m_mainScene->keyUp(windowEvent.key.code);
					break;
				case sf::Event::EventType::MouseWheelScrolled:

					m_mainScene->mouseController.screenCoords = Vector2D(
						windowEvent.mouseWheelScroll.x,
						windowEvent.mouseWheelScroll.y
					);

					m_mainScene->mouseController.globalCoords = Vector2D(
						windowEvent.mouseWheelScroll.x,
						windowEvent.mouseWheelScroll.y
					);

					m_mainScene->mouseController.globalCoords = m_renderWindow->mapPixelToCoords((sf::Vector2i)(sf::Vector2f)m_mainScene->mouseController.globalCoords);

					if (pointOverView) {
						m_mainScene->scroll(windowEvent.mouseWheelScroll.delta);
					}
					break;
				default:
					break;
				}
			}

			//CLEAR THE WINDOW
			m_renderWindow->clear((sf::Color)m_properties.backgroundColor);
			m_renderTexture->clear((sf::Color)m_properties.backgroundColor);
			
			sf::Vector2f viewSize = (sf::Vector2f)m_viewportWindow->getProperties().size;
			sf::Vector2f viewPosition = (sf::Vector2f)m_viewportWindow->getProperties().position;

			
			if (!PointRect(m_mainScene->mouseController.screenCoords, (VIDEKit::Vector2D)viewPosition + VIDEKit::Vector2D(10, 30), (VIDEKit::Vector2D)viewSize - VIDEKit::Vector2D(20, 35))) {
				m_mainScene->mouseController.clear();
				m_viewportWindow->control.moving = false;
			}
			else {
				if (m_gui->control.hoveredWindows > 10 || displayVariableModal) {
					m_mainScene->mouseController.clear();
					m_viewportWindow->control.moving = false;
				}
				else {
					m_viewportWindow->control.moving = true;
				}
			}

			//UPDATE THE CAMERA
			m_mainScene->getCamera()->update();
			//UPDATE THE VIEW
			m_renderTexture->setView(*m_mainScene->getCamera()->getView());

			//RENDER THE SCENE
			m_mainScene->render();

			//DRAW THE GUI ON THE TOP
			m_gui->update(m_mainScene, deltaClock.restart());

			m_renderWindow->display();
			m_renderTexture->display();


		} while (!m_properties.closed);

		if (m_graphProcessor->thread() != nullptr) {
			if (m_graphProcessor->thread()->joinable()) {
				m_graphProcessor->thread()->join();	//WHAIT UNTIL CREATED PROGRAM TERMINATE
			}
		}

		m_renderWindow->close();
	}

	VIDEKit::Variable * Window::addVariable(VIDEGui::TreeView::ChildItem * guiReference, std::string name)
	{
		VIDEKit::Variable *createdVariable = new VIDEKit::Variable(guiReference);
		
		if (guiReference != nullptr) {
			createdVariable->setName(guiReference->dragTagName);
		}
		else {
			createdVariable->setName(name);
		}
		
		createdVariable->setType(videc->getDataTypes()[0]->typeName);
		createdVariable->setValue(videc->getDataTypes()[0]->defaultTypeValue);

		m_variables.push_back(createdVariable);

		return createdVariable;
	}

	/*
		RETURN THE CURRENT WINDOW PROPERTIES
	*/
	WindowProperties Window::getProperties()
	{
		WindowProperties ret{};

		ret.title = m_properties.title;
		ret.backgroundColor = m_properties.backgroundColor;
		ret.width = m_renderWindow->getSize().x;
		ret.height = m_renderWindow->getSize().y;
		ret.closed = !m_renderWindow->isOpen();
		return ret;
	}

	/*
		RETURN THE SFML WINDOW REFERENCE(frame render)
	*/
	sf::RenderWindow *Window::frame() {
		return this->m_renderWindow;
	}

	sf::RenderTexture *Window::texture() {
		return this->m_renderTexture;
	}

	VIDEKit::Scene *Window::getScene() {
		return m_mainScene;
	}

	std::vector<VIDEKit::Variable*> &Window::getVariables()
	{
		return m_variables;
	}

	VIDEKit::Variable * Window::getVariable(const std::string & variableName)
	{
		for (int i = 0; i < m_variables.size(); i++) {
			if (m_variables[i]->getName().compare(variableName) == 0) {
				return m_variables[i];
			}
		}

		return nullptr;
	}

	VIDEGui::GraphicsUI * Window::getGUI()
	{
		return m_gui;
	}

	void Window::load(const std::string &path) {
		m_graphProcessor->load(path);
	}

	void Window::save(const std::string & path)
	{
		m_graphProcessor->save(path);
	}

	void Window::loadPlugins()
	{
		//get all the videc files as plugins for user defined types

		boost::filesystem::path p(boost::filesystem::current_path().string() + "\\plugins\\");

		if (is_directory(p)) {
			for (auto& entry : boost::make_iterator_range(boost::filesystem::directory_iterator(p), {})) {
				
				VIDECLexer lexer;

				if (!lexer.fromFile(entry.path().string())) {
					std::cout << "Couldn't load plugin \"" << entry.path().stem().string() << "\", Syntax Error..." << std::endl;
					std::cout << "VIDEKit will continue without data-types defined on this plugin..." << std::endl;
				}
				else {
					videc->getBehaviors().insert(videc->getBehaviors().end(), lexer.getBehaviors().begin(), lexer.getBehaviors().end());
					videc->getSections().insert(videc->getSections().end(), lexer.getSections().begin(), lexer.getSections().end());
					videc->getDataTypes().insert(videc->getDataTypes().end(), lexer.getDataTypes().begin(), lexer.getDataTypes().end());

					std::cout << "VIDEKit plugin \"" << entry.path().stem().string() << "\" loaded successfully." << std::endl;
				}
			}
		}
	}
}