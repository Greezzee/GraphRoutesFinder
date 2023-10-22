#pragma once
#include <list>
#include "Vector2D.h"
#include "Circle2D.h"

namespace voronoi {

template <typename CoordType>
class Wall 
{

	using point_t = Vector2D<CoordType>;

	// TODO:
	// Добавить свойство, характеризующее препятствие как "тонкий объект". Если его толщина ниже порога - особая обработка
	// ( Заменить 2 грани на малених расстояних одной )

public:

	Wall(): m_isClosed(false), m_isInnerFill(true) {}

	void addPoint(const point_t& newPoint) { m_points.push_back(newPoint); }

	std::list<point_t> getPoints() {
		return m_points;
	}

	void addInnerEquidistantPoints(CoordType distance) {
		if (m_isClosed)
			m_points.push_back(m_points.front());
		auto curPointIt = m_points.begin();
		
		while (std::next(curPointIt) != m_points.end()) {
			point_t nextPoint = *std::next(curPointIt);
			point_t curPoint = *curPointIt;
			int pointCount = std::ceil((nextPoint - curPoint).lenght() / distance);
			CoordType newDistance = (nextPoint - curPoint).lenght() / pointCount;
			auto direction = (nextPoint - curPoint).normalize();
			while ( (nextPoint - curPoint).lenghtSqr() > newDistance * newDistance + 0.001) {
				point_t newPoint = curPoint + direction * newDistance;

				m_points.insert(std::next(curPointIt), newPoint);
				curPointIt = std::next(curPointIt);
				curPoint = *curPointIt;
			}
			curPointIt = std::next(curPointIt);
		}
		if (m_isClosed)
			m_points.pop_back();
	}

	void reducePoints(CoordType minDistanceBetweenPoints) {
		if (m_isClosed)
			m_points.push_back(m_points.front());

		for (auto it = m_points.begin(), ite = m_points.end(); std::next(it) != ite;) {
			auto start = it;
			auto finish = std::next(it);

			CoordType distance = (*finish - *start).lenght();

			while (distance < minDistanceBetweenPoints && std::next(finish) != ite) {
				distance += (*std::next(finish) - *finish).lenght();

				finish = std::next(finish);
				m_points.erase(std::prev(finish));
			}
			it = finish;
		}

		for (auto it = std::prev(m_points.end()), ite = m_points.begin(); std::prev(it) != ite;) {
			auto start = it;
			auto finish = std::prev(it);

			CoordType distance = (*finish - *start).lenght();

			while (distance < minDistanceBetweenPoints && std::prev(finish) != ite) {
				distance += (*std::prev(finish) - *finish).lenght();

				finish = std::prev(finish);
				m_points.erase(std::next(finish));
			}
			it = finish;
		}

		if (m_isClosed)
			m_points.pop_back();
	}

	std::vector<point_t> generateAdditionalPointsForVoronoi(CoordType distanceToWall) {
		std::vector<point_t> outPoints;

		if (m_isClosed)
			m_points.push_back(m_points.front());

		for (auto it = m_points.begin(), ite = m_points.end(); std::next(it) != ite; ++it) {
			Circle2D<CoordType> a, b;
			a.center = *it;
			b.center = *std::next(it);

			CoordType distanceSqr = (a.center - b.center).lenghtSqr() / 4.;
			CoordType circlesRadius = std::sqrt(distanceSqr + distanceToWall * distanceToWall);

			a.radius = b.radius = circlesRadius;

			auto interPoints = circleIntersection(a, b);

			outPoints.insert(outPoints.end(), interPoints.points.begin(), interPoints.points.end());
		}

		if (m_isClosed)
			m_points.pop_back();

		return outPoints;
	}

	const std::list<point_t>& getPoints() const { return m_points; }

	void setIsClosed(bool isClosed) { m_isClosed = isClosed; }

	void setIsInnerFill(bool innerFill) { m_isInnerFill = innerFill; }

	bool isClosed() const { return m_isClosed; }

	bool isPointInside(const point_t& testingPoint) const {
		if (!m_isClosed)
			return false;

		m_points.push_back(m_points.front());

		size_t crossingNumberCounter = 0;

		for (auto it = m_points.begin(), ite = m_points.end(); std::next(it) != ite; ++it) {
			auto& pointA = *it;
			auto& pointB = *std::next(it);

			if (pointA.y <= testingPoint.y && pointB.y > testingPoint.y || pointB.y <= testingPoint.y && pointA.y > testingPoint.y) {
				CoordType vt = (testingPoint.y - pointA.y) / (pointB.y - pointA.y);
				if (testingPoint.x < pointA.x + vt * (pointB.x - pointA.x))
					crossingNumberCounter++;
			}
		}

		m_points.pop_back();

		if (m_isInnerFill)
			return crossingNumberCounter % 2 == 1;
		return crossingNumberCounter % 2 == 0;
	}

private:
	mutable std::list<point_t> m_points;
	bool m_isClosed, m_isInnerFill;
};

} // namespace voronoi