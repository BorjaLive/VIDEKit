#pragma once

//FRAMEWORK COMPONENTS
#include "Shape.h"

//SFML
#include "RoundedRectangleShape.h"

//EXTRA
#include "../Transform.h"

namespace VIDEKit {

	class Rectangle : public Shape
	{
		sf::RoundedRectangleShape m_shape;
	public:
		Rectangle(const Transform &transform, const Size2D &size);
		~Rectangle();

		virtual void draw(SFMLRenderWindow* window) override;
		virtual void draw(SFMLRenderTexture* texture) override;

		void updateBounds();

	public:
		float cornerRadius;
	};

}