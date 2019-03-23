#include "Vector2D.h"


#define _USE_MATH_DEFINES
#include <math.h>

namespace VIDEKit {

	Vector2D::Vector2D():x(0.0f), y(0.0f){}

	Vector2D::Vector2D(float _x, float _y) : x(_x), y(_y){}

	Vector2D::Vector2D(const sf::Vector2f& v) : x(v.x), y(v.y) {}

	Vector2D::~Vector2D(){
		//DTOR
	}

	//SFML CONVERSION
	Vector2D::operator sf::Vector2f() {
		return sf::Vector2f(x,y);
	}

	//VECTOR OPERATOR VECTOR

	Vector2D Vector2D::operator+(const Vector2D &u){
		return Vector2D(x + u.x, y + u.y);
	}
	Vector2D Vector2D::operator-(const Vector2D &u){
		return Vector2D(x - u.x, y - u.y);
	}
	Vector2D Vector2D::operator*(const Vector2D &u){
		return Vector2D(x * u.x, y * u.y);
	}
	Vector2D Vector2D::operator/(const Vector2D &u){
		return Vector2D(x / u.x, y / u.y);
	}

	//VECTOR OPERATOR FLOAT

	Vector2D Vector2D::operator+(const float& num){
		return Vector2D(x + num, y + num);
	}
	Vector2D Vector2D::operator-(const float& num){
		return Vector2D(x - num, y - num);
	}
	Vector2D Vector2D::operator*(const float& num){
		return Vector2D(x * num, y * num);
	}
	Vector2D Vector2D::operator/(const float& num) {
		return Vector2D(x / num, y / num);
	}


	//INLINE VECTOR OPERATOR EQUAL VECTOR

	Vector2D Vector2D::operator+=(const Vector2D& u){
		x += u.x;
		y += u.y;
		return *this;
	}
	Vector2D Vector2D::operator-=(const Vector2D& u){
		x += u.x;
		y += u.y;
		return *this;
	}
	Vector2D Vector2D::operator*=(const Vector2D& u){
		x += u.x;
		y += u.y;
		return *this;
	}
	Vector2D Vector2D::operator/=(const Vector2D& u) {
		x += u.x;
		y += u.y;
		return *this;
	}

	//INLINE VECTOR OPERATOR EQUAL NUMBER

	Vector2D Vector2D::operator+=(const float& num){
		x += num;
		y += num;
		return *this;
	}
	Vector2D Vector2D::operator-=(const float& num){
		x -= num;
		y -= num;
		return *this;
	}
	Vector2D Vector2D::operator*=(const float& num){
		x *= num;
		y *= num;
		return *this;
	}
	Vector2D Vector2D::operator/=(const float& num) {
		x /= num;
		y /= num;
		return *this;
	}

	Vector2D Vector2D::operator=(const sf::Vector2f &v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}

	bool Vector2D::operator==(const Vector2D &other)
	{
		return (this->x == other.x && this->y == other.y);
	}

	bool Vector2D::operator!=(const Vector2D &other)
	{
		return (this->x != other.x || this->y != other.y);
	}

	/**
		UTILS
	*/

	float Vector2D::length() const{
		return sqrtf(powf(x, 2) + powf(y, 2));
	}
	float Vector2D::Length(const Vector2D &v) {
		return v.length();
	}

	void Vector2D::normalize() {
		*this /= length();
	}
	Vector2D Vector2D::normalized() {
		return *this / length();
	}
	Vector2D Vector2D::normalized(Vector2D v) {
		return v/v.length();
	}

	float Vector2D::angle(bool radians) const{
		return atan2(y,x) * ((radians) ? 1.0f : (180.0f / M_PI));
	}
	float Vector2D::Angle(const Vector2D &v, bool radians) {
		return v.angle() * ((radians) ? 1.0f : (180.0f/M_PI));
	}


	float Vector2D::angleTo(const Vector2D &v, bool radians) const{
		return (v.angle() - angle()) * ((radians) ? 1.0f : (180.0f / M_PI));
	}
	float Vector2D::AngleTo(const Vector2D &v, const Vector2D &u, bool radians) {
		return v.angleTo(u) * ((radians) ? 1.0f : (180.0f / M_PI));
	}


	float Vector2D::dot(const Vector2D &v) const{
		return (x * v.x) + (y * v.y);
	}
	float Vector2D::Dot(const Vector2D &v, const Vector2D &u) {
		return v.dot(u);
	}


	Vector2D Vector2D::left() {
		return Vector2D(-y,x);
	}
	Vector2D Vector2D::Left(const Vector2D &v) {
		return Vector2D(-v.y, v.x);
	}


	Vector2D Vector2D::right() {
		return Vector2D(y,-x);
	}
	Vector2D Vector2D::Right(const Vector2D &v) {
		return Vector2D(v.y, -v.x);
	}

	void Vector2D::VInterp(const Vector2D &u, float t) {
		t = fminf(1.0f, fmaxf(0.0f, t));
		x = x + ((u.x - x)*t);
		y = y + ((u.y - y)*t);
	}
	Vector2D Vector2D::VInterp(const Vector2D &v, const Vector2D &u, float t) {
		t = fminf(1.0f, fmaxf(0.0f, t));
		return Vector2D(v.x + ((u.x - v.x)*t), v.y + ((u.y - v.y)*t));
	}
}