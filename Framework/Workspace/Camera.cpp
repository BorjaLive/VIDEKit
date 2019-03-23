#include "Camera.h"

//FRAMEWORK COMPONENTS
#include "../Window.h"
#include "../../Framework/Workspace/Scene.h"

//SFML
#include <SFML/Graphics/View.hpp>


namespace VIDEKit {

	Camera::Camera(VIDEKit::Scene *attachedScene)
	{
		m_scene = attachedScene;

		m_view = new sf::View(m_scene->getWindow()->frame()->getDefaultView());

		WindowProperties attachedWindowProperties = m_scene->getWindow()->getProperties();

		m_properties = CameraProperties();
		m_properties.targetPosition = VIDEKit::Vector2D(0,0);
		m_properties.currentPosition = m_properties.targetPosition;
		m_properties.size = VIDEKit::Vector2D((float)attachedWindowProperties.width,(float)attachedWindowProperties.height);
		m_properties.zoom = 1.0f;
		m_properties.minZoom = 1.0f;
		m_properties.maxZoom = 5.0f;

		this->update();
	}


	Camera::~Camera()
	{
	}

	CameraProperties &Camera::getProperties() {
		return m_properties;
	}

	SFMLView *Camera::getView() {
		return m_view;
	}

	void Camera::update() {

		m_properties.currentPosition.VInterp(m_properties.targetPosition, 0.1f);
		
		m_properties.size = (sf::Vector2f)m_scene->getWindow()->frame()->getSize();

		m_properties.zoom = fmaxf(m_properties.minZoom, fminf(m_properties.zoom, m_properties.maxZoom));

		m_view->setCenter((sf::Vector2f)m_properties.currentPosition);
		m_view->setSize((sf::Vector2f)m_properties.size);
		m_view->zoom(m_properties.zoom);
	}

	void Camera::move(const Vector2D &dir) {
		m_properties.targetPosition += dir;
	}

	void Camera::setPosition(const Vector2D &position) {
		m_properties.targetPosition = position;
	}
}