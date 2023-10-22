#pragma once
#define PI 3.1415926
#include <cmath>

namespace voronoi {

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

template <typename T>
struct Vector2D 
{
	using CoordType = T;

	T x, y;
	Vector2D() {};
	Vector2D(double a, double b): x(a), y(b) {}

	Vector2D(const Vector2D<T>& other) : x(other.x), y(other.y) {}

	Vector2D<T>& operator=(const Vector2D<T>& other) {
		x = other.x;
		y = other.y;
		return *this;
	}

	virtual ~Vector2D() {}

	T lenghtSqr() const {
		return x * x + y * y;
	}
	T lenght() const {
		return std::sqrt(lenghtSqr());
	}
	Vector2D& normalize() & {
		*this /= lenght();
		return *this;
	}

	Vector2D&& normalize() && {
		return std::move(normalize());
	}

	Vector2D& rotate(double angle)& {
		T old_x = x, old_y = y;
		x = old_x * std::cos(angle) - old_y * std::sin(angle);
		y = old_x * std::sin(angle) + old_y * std::cos(angle);
		return *this;
	}

	Vector2D&& rotate(double angle)&& {
		return std::move(rotate(angle));
	}

	double getAngle() {
		if (x == 0 && y == 0)
			return 0;
		if (x == 0)
			return PI / 2 * sgn(y);
		return std::atan2(y, x);
	}



	Vector2D& operator+=(const Vector2D& rhs) & {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	Vector2D& operator-=(const Vector2D& rhs) & {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}
	Vector2D& operator*=(const T& rhs) & {
		x *= rhs;
		y *= rhs;
		return *this;
	}
	Vector2D& operator/=(const T& rhs) & {
		x /= rhs;
		y /= rhs;
		return *this;
	}
};

// Segment is a vector with offset
template <typename vector_t>
struct Segment2D : vector_t
{
	using CoordType = typename vector_t::CoordType;
	vector_t offset;
};

template <typename T>
bool operator==(const Vector2D<T>& lhs, const Vector2D<T>& rhs) {
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

template <typename T>
Vector2D<T> operator+(const Vector2D<T>& lhs, const Vector2D<T>& rhs) {
	auto tmp = lhs;
	return tmp += rhs;
}

template <typename T>
Vector2D<T> operator-(const Vector2D<T>& lhs, const Vector2D<T>& rhs) {
	auto tmp = lhs;
	return tmp -= rhs;
}

template <typename T>
Vector2D<T> operator*(const Vector2D<T>& lhs, const T& rhs) {
	auto tmp = lhs;
	return tmp *= rhs;
}

template <typename T>
Vector2D<T> operator/(const Vector2D<T>& lhs, const T& rhs) {
	auto tmp = lhs;
	return tmp /= rhs;
}

template <typename T>
Vector2D<T> operator*(const T& lhs, const Vector2D<T>& rhs) {
	auto tmp = rhs;
	return tmp *= lhs;
}

template <typename T>
Vector2D<T> operator/(const T& lhs, const Vector2D<T>& rhs) {
	auto tmp = rhs;
	return tmp /= lhs;
}

template <typename T>
T dotProduct(const Vector2D<T>& lhs, const Vector2D<T>& rhs) {
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

} // namespace voronoi