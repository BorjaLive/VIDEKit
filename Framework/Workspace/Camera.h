#pragma once

//SFML
#include <SFML/Graphics/View.hpp>

//EXTRA
#include "../../Extra/Vector2D.h"

using SFMLView = sf::View;

namespace VIDEKit {

	struct CameraProperties {
		Vector2D currentPosition;
		Vector2D targetPosition;
		Vector2D size;
		float zoom;
		float maxZoom;
		float minZoom;
	};

	class Camera
	{
	private:
		SFMLView *m_view;
		class Scene *m_scene;

		CameraProperties m_properties;

	public:
		Camera(class Scene *);
		~Camera();

		CameraProperties &getProperties();
		SFMLView *getView();

		void update();

		void move(const Vector2D &dir);
		void setPosition(const Vector2D &position);
	};

}