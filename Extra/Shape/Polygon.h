#pragma once

#include "Shape.h"

namespace VIDEKit {

	class Polygon : public Shape
	{
	private:
		sf::ConvexShape m_shape;
		std::vector<Vector2D> points;

	public:
		Polygon(std::vector<Vector2D> p);
		~Polygon();

		virtual void draw(SFMLRenderWindow *window) override;
		virtual void draw(SFMLRenderTexture *texture) override;

		void setPoints(std::vector<Vector2D> p);
	};

}