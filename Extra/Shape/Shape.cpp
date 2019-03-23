#include "Shape.h"



namespace VIDEKit {


	Shape::Shape()
	{
		m_transform = VIDEKit::Transform();
		fillColor = Color(255,255,255);
		borderColor = Color(0,0,0,0);
		borderWidth = 0.0f;
	}

	Shape::Shape(const VIDEKit::Transform& transform) {
		m_transform = transform;
	}

	Shape::~Shape()
	{
	}

	VIDEKit::Transform *Shape::getTransform() {
		return &m_transform;
	}
}