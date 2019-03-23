#include "Polygon.h"

namespace VIDEKit {

	Polygon::Polygon(std::vector<Vector2D> p)
	{
		points = p;
	}


	Polygon::~Polygon(){}

	void Polygon::draw(SFMLRenderWindow* window) {
		window->draw(m_shape);
	}

	void Polygon::draw(SFMLRenderTexture* texture) {
		texture->draw(m_shape);
	}

	void Polygon::setPoints(std::vector<Vector2D> p) {
		points = p;

		m_shape = sf::ConvexShape();
		m_shape.setPointCount(p.size());

		for (int i = 0; i < p.size(); i++) {
			m_shape.setPoint(i, (sf::Vector2f)p[i]);
		}

		m_shape.setFillColor((sf::Color)fillColor);
		m_shape.setOutlineColor((sf::Color)borderColor);
		m_shape.setOutlineThickness(borderWidth);
	}
}