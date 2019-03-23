#pragma once

#include "Shape.h"

//SFML
#include <SFML/Graphics/RectangleShape.hpp>

namespace VIDEKit {

	class Line : public VIDEKit::Shape
	{
	public:
		float thickness;
		std::vector<Vector2D> points;

	public:
		Line(std::vector<Vector2D> p);
		~Line();

		static std::vector<Vector2D> bezierCurve(std::vector<Vector2D> controlPoints);
		static Vector2D bezierCurvePoint(std::vector<Vector2D> control, float delta);

		virtual void draw(SFMLRenderWindow *window) override;
		virtual void draw(SFMLRenderTexture *texture) override;
	};

}