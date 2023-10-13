#pragma once
#include <utility>
#include <vector>
#include "Vector2D.h"

namespace voronoi {

template <typename CoordType>
struct Intercsetion {
	bool isInfinite;
	size_t pointsCount;
	std::vector<Vector2D<CoordType>> points;
};

template <typename CoordType>
struct Circle2D 
{
	using point_t = Vector2D<CoordType>;
	point_t center;
	CoordType radius; 

	bool isPointInside(const point_t& point) {
		return (point - radius).lenghtSqr < radius * radius;
	}
};

template <typename CoordType>
bool operator== (const Circle2D<CoordType>& lhs, const Circle2D<CoordType>& rhs) {
	return lhs.center == rhs.center && lhs.radius == rhs.radius;
}

template <typename CoordType>
Intercsetion<CoordType> circleIntersection(Circle2D<CoordType> lhs, Circle2D<CoordType> rhs) {
	Intercsetion<CoordType> out;
	if (lhs == rhs) {
		out.isInfinite = true;
		return out;
	}
	auto lineBetweenCenters = rhs.center - lhs.center;

	CoordType d = lineBetweenCenters.lenght();
	if (d == 0) {
		out.isInfinite = false;
		out.pointsCount = 0;
		return out;
	}

	CoordType x = (d * d - rhs.radius * rhs.radius + lhs.radius * lhs.radius) / (2 * d);
	if (x > lhs.radius) {
		out.isInfinite = false;
		out.pointsCount = 0;
		return out;
	}

	CoordType y = std::sqrt(lhs.radius * lhs.radius - x * x);

	lineBetweenCenters.normalize();
	auto ortoLineBetweenCenters = lineBetweenCenters;
	ortoLineBetweenCenters.rotate(PI / 2);
	auto a = lhs.center + lineBetweenCenters * x + ortoLineBetweenCenters * y;
	auto b = lhs.center + lineBetweenCenters * x - ortoLineBetweenCenters * y;

	if (a == b) {
		out.isInfinite = false;
		out.pointsCount = 1;
		out.points.push_back(a);
		return out;
	}

	out.isInfinite = false;
	out.pointsCount = 2;
	out.points.push_back(a);
	out.points.push_back(b);
	return out;
}

} // namespace voronoi