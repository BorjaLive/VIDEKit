#include "Transform.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace VIDEKit {

	Transform::Transform():position(Vector2D()), size(Size2D()), rotation(0.0f), scale(Vector2D(1.0f, 1.0f)){}
	Transform::Transform(const Vector2D& p, const float& r) : position(p), size(Size2D()), rotation(r), scale(Vector2D(1.0f, 1.0f)) {}
	Transform::Transform(const Size2D& sz, const float&r) : position(Vector2D()), size(sz), rotation(r), scale(Vector2D(1.0f, 1.0f)) {}
	Transform::Transform(const Vector2D& p, const Size2D& sz, const float& r) : position(p), size(sz), rotation(r), scale(Vector2D(1.0f, 1.0f)) {}
	Transform::Transform(const float& r) : position(Vector2D()), size(Size2D()), rotation(r), scale(Vector2D(1.0f, 1.0f)) {}

	Transform::~Transform(){}

	Vector2D Transform::getUpVector() {
		return Vector2D(cosf(rotation*M_PI/180.0f), sinf(rotation*M_PI / 180.0f));
	}
	Vector2D Transform::getDownVector() {
		return Vector2D(cosf(rotation*M_PI / 180.0f), sinf(rotation*M_PI / 180.0f))*-1.0;
	}
	Vector2D Transform::getRightVector() {
		return Vector2D(cosf(rotation*M_PI / 180.0f), sinf(rotation*M_PI / 180.0f)).right();
	}
	Vector2D Transform::getLeftVector() {
		return Vector2D(cosf(rotation*M_PI / 180.0f), sinf(rotation*M_PI / 180.0f)).left();
	}

	void Transform::setPosition(const Vector2D &position) {
		this->position = position;
	}
	void Transform::Translate(const Vector2D &direction) {
		this->position += direction;
	}

	void Transform::setRotation(const float &angle) {
		this->rotation = angle;
	}
	void Transform::Rotate(const float &angle) {
		this->rotation += angle;
	}

	void Transform::setScale(const Vector2D &scale) {
		this->scale = scale;
	}
	void Transform::setScale(const float &scale) {
		this->scale = Vector2D(scale, scale);
	}
	void Transform::Scale(const float &scale) {
		this->scale += scale;
	}
}