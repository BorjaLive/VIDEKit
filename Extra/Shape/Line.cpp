#include "Line.h"

#include "../../Extra/Utils.h"

namespace VIDEKit {

	Line::Line(std::vector<Vector2D> p) : Shape()
	{
		this->points = p;
		this->thickness = 2;
	}


	Line::~Line()
	{
	}

	std::vector<Vector2D> Line::bezierCurve(std::vector<Vector2D> controlPoints)
	{
		std::vector<Vector2D> ret(100);

		int i = 0;

		for (float delta = 0.0f; delta < 0.99f; delta += 0.01f) {
			ret[i] = bezierCurvePoint(controlPoints, delta) - controlPoints[0];
			i++;
		}

		return ret;
	}

	Vector2D Line::bezierCurvePoint(std::vector<Vector2D> control, float delta)
	{
		Vector2D ret = control[0];


		int n = (int)control.size() - 1;

		for (int i = 0; i <= n; i++) {
			float c = nOverK(n, i);

			ret.x += c * powf(1.0f - delta, (float)n - (float)i) * powf(delta, (float)i) * control[i].x;
			ret.y += c * powf(1.0f - delta, (float)n - (float)i) * powf(delta, (float)i) * control[i].y;
		}

		return ret;
	}

	void Line::draw(SFMLRenderWindow *wnd) {
		
		for (int i = 1; i < points.size(); i++) {
			sf::RectangleShape line;

			Vector2D dir = points[i] - points[i-1];

			float len = dir.length();

			line.setOrigin(0.0f, thickness / 2.0f);
			line.setSize(sf::Vector2f(len, thickness));
			line.setPosition((sf::Vector2f)(points[i-1]));
			line.setRotation(dir.angle(false));
			line.setFillColor((sf::Color)fillColor);
			line.setOutlineColor((sf::Color)borderColor);
			line.setOutlineThickness(borderWidth);
			wnd->draw(line);
		}
	}

	void Line::draw(SFMLRenderTexture *texture) {

		for (int i = 1; i < points.size(); i++) {
			sf::RectangleShape line;

			Vector2D dir = points[i] - points[i - 1];

			float len = dir.length();

			line.setOrigin(0.0f, thickness / 2.0f);
			line.setSize(sf::Vector2f(len, thickness));
			line.setPosition((sf::Vector2f)(points[i - 1]));
			line.setRotation(dir.angle(false));
			line.setFillColor((sf::Color)fillColor);
			line.setOutlineColor((sf::Color)borderColor);
			line.setOutlineThickness(borderWidth);
			texture->draw(line);
		}
	}
}