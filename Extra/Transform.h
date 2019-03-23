#pragma once

#include "Vector2D.h"

namespace VIDEKit {


	class Transform
	{
	public:
		Vector2D position;
		Size2D size;
		Vector2D scale;
		float rotation;

	public:
		Transform();
		Transform(const Vector2D&, const float& = 0.0f);
		Transform(const Size2D&, const float& = 0.0f);
		Transform(const Vector2D&, const Size2D&, const float & = 0.0f);
		Transform(const float&);

		~Transform();

		Vector2D getUpVector();
		Vector2D getDownVector();
		Vector2D getRightVector();
		Vector2D getLeftVector();

		void setPosition(const Vector2D &position);
		void Translate(const Vector2D &direction);

		void setRotation(const float &angle);
		void Rotate(const float &angle);

		void setScale(const Vector2D &scale);
		void setScale(const float &scale);
		void Scale(const float &scale);
	};

}