#pragma once

#include <SFML/System/Vector2.hpp>
#include <iostream>

namespace VIDEKit {
	
	class Vector2D
	{
	public:
		///CTOR/DTOR
		Vector2D();
		Vector2D(float _x, float _y);	
		Vector2D(const sf::Vector2f&);
		~Vector2D();

		/*
			SFML TYPE CONVERSION
		*/
		explicit operator sf::Vector2f();

		/**
			OPERATORS
		*/

		Vector2D operator+(const Vector2D&);
		Vector2D operator-(const Vector2D&);
		Vector2D operator*(const Vector2D&);
		Vector2D operator/(const Vector2D&);

		Vector2D operator+(const float&);
		Vector2D operator-(const float&);
		Vector2D operator*(const float&);
		Vector2D operator/(const float&);


		Vector2D operator+=(const Vector2D&);
		Vector2D operator-=(const Vector2D&);
		Vector2D operator*=(const Vector2D&);
		Vector2D operator/=(const Vector2D&);

		Vector2D operator+=(const float&);
		Vector2D operator-=(const float&);
		Vector2D operator*=(const float&);
		Vector2D operator/=(const float&);

		Vector2D operator=(const sf::Vector2f&);

		bool operator==(const Vector2D &);
		bool operator!=(const Vector2D &);

		/**
			UTILS
		*/

		float length() const;
		static float Length(const Vector2D&);

		void normalize();
		Vector2D normalized();
		static Vector2D normalized(Vector2D);

		float angle(bool=true) const;
		static float Angle(const Vector2D &, bool=true);
		
		float angleTo(const Vector2D &,bool=true) const;
		static float AngleTo(const Vector2D &, const Vector2D &, bool=true);

		float dot(const Vector2D&) const;
		static float Dot(const Vector2D &, const Vector2D &);

		Vector2D left();
		static Vector2D Left(const Vector2D &);

		Vector2D right();
		static Vector2D Right(const Vector2D &);

		void VInterp(const Vector2D &, float);
		static Vector2D VInterp(const Vector2D &, const Vector2D &, float);

	public:
		float x, y;
	};

	struct Size2D {
		float width;
		float height;

		Size2D():width(0),height(0){}
		Size2D(float w, float h):width(w), height(h){}
	};

	typedef Vector2D Point2D;

	static std::ostream& operator<<(std::ostream& stream, const VIDEKit::Vector2D &v) {
		stream << "Vector2D(" << v.x << ", " << v.y << ")";
		return stream;
	}

	static std::ostream& operator<<(std::ostream& stream, const VIDEKit::Size2D &sz) {
		stream << "Size2D(" << sz.width << ", " << sz.height << ")";
		return stream;
	}
}