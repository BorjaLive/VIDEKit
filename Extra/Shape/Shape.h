#pragma once

//EXTRA
#include "../../Extra/Transform.h"
#include "../../Extra/Color.h"

#include <SFML/Graphics.hpp>

using SFMLRenderWindow = sf::RenderWindow;
using SFMLRenderTexture = sf::RenderTexture;

namespace VIDEKit {

	class Shape
	{
	protected:
		VIDEKit::Transform m_transform;

	public:
		VIDEKit::Color fillColor;
		VIDEKit::Color borderColor;
		float borderWidth;
	public:
		Shape();
		Shape(const VIDEKit::Transform& transform);
		~Shape();

		virtual void draw(SFMLRenderWindow *window) = 0;
		virtual void draw(SFMLRenderTexture *texture) = 0;

		VIDEKit::Transform *getTransform();
	};

}