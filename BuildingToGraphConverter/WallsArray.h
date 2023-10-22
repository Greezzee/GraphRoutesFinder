#pragma once

#include <vector>
#include "Wall.h"

namespace voronoi {

template <typename CoordType>
class WallsArray : public std::vector<Wall<CoordType>>
{

	using point_t = Vector2D<CoordType>;

public:
	bool isPointInsideWall(const point_t& testingPoint) const {
		for (size_t i = 0, end = this->size(); i < end; ++i) {
			if (this->at(i).isPointInside(testingPoint))
				return true;
		}
		return false;
	}
};



} // namespace voronoi