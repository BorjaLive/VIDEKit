#include "Rectangle.h"

namespace VIDEKit {

	Rectangle::Rectangle(const Transform& transform, const Size2D &size){
		m_shape = sf::RoundedRectangleShape();
		m_transform = transform;
		m_transform.size = size;
		cornerRadius = 0.0f;
		updateBounds();
	}


	Rectangle::~Rectangle()
	{
	}

	void Rectangle::draw(SFMLRenderWindow* window) {
		this->updateBounds();
		window->draw(m_shape);
	}

	void Rectangle::draw(SFMLRenderTexture* texture) {
		this->updateBounds();
		texture->draw(m_shape);
	}

	void Rectangle::updateBounds() {
		m_shape.setCornerPointCount((cornerRadius == 0.0f) ? 2 : fminf(cornerRadius, 20));
		m_shape.setCornersRadius(cornerRadius);

		m_shape.setSize(sf::Vector2f(m_transform.size.width, m_transform.size.height));

		m_shape.setOrigin(m_transform.size.width/2.0, m_transform.size.height/2.0);

		m_shape.setPosition(m_transform.position.x, m_transform.position.y);
		m_shape.setRotation(m_transform.rotation);
		m_shape.setScale((sf::Vector2f)m_transform.scale);
		
		
		m_shape.setFillColor((sf::Color)fillColor);
		m_shape.setOutlineThickness(borderWidth);
		m_shape.setOutlineColor((sf::Color)borderColor);
	}

}