#pragma once
#include <list>
#include "Vector2D.h"
#include "Circle2D.h"

namespace voronoi {

template <typename CoordType>
class Wall 
{

	using point_t = Vector2D<CoordType>;

public:

	Wall(): m_isClosed(false), m_isInnerFill(true) {}

	void addPoint(const point_t& newPoint) { m_points.push_back(newPoint); }

	void addInnerEquidistantPoints(CoordType distance) {
		auto curPointIt = m_points.begin();
		
		while (std::next(curPointIt) != m_points.end()) {
			point_t nextPoint = *std::next(curPointIt);
			point_t curPoint = *curPointIt;
			int pointCount = std::ceil((nextPoint - curPoint).lenght() / distance);
			CoordType newDistance = (nextPoint - curPoint).lenght() / pointCount;
			auto direction = (nextPoint - curPoint).normalize();
			while ( (nextPoint - curPoint).lenghtSqr() > newDistance * newDistance) {
				point_t newPoint = curPoint + direction * newDistance;

				m_points.insert(std::next(curPointIt), newPoint);
				curPointIt = std::next(curPointIt);
				curPoint = *curPointIt;
			}
			curPointIt = std::next(curPointIt);
		}
	}

	std::vector<point_t> generateAdditionalPointsForVoronoi(CoordType circlesRadius) {
		std::vector<point_t> outPoints;

		for (auto it = m_points.begin(), ite = m_points.end(); std::next(it) != ite; ++it) {
			Circle2D<CoordType> a, b;
			a.center = *it;
			b.center = *std::next(it);
			a.radius = b.radius = circlesRadius;

			auto interPoints = circleIntersection(a, b);

			outPoints.insert(outPoints.end(), interPoints.points.begin(), interPoints.points.end());
		}

		return outPoints;
	}

	const std::list<point_t>& getPoints() const { return m_points; }
private:
	std::list<point_t> m_points;
	bool m_isClosed, m_isInnerFill;
};

} // namespace voronoi