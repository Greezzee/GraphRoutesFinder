#include "boost/polygon/voronoi.hpp"

using boost::polygon::voronoi_builder;
using boost::polygon::voronoi_diagram;

struct Point {
	double a;
	double b;
	Point(double x, double y) : a(x), b(y) {}
};

namespace boost {
	namespace polygon {

		template <>
		struct geometry_concept<Point> {
			typedef point_concept type;
		};

		template <>
		struct point_traits<Point> {
			typedef double coordinate_type;

			static inline coordinate_type get(
				const Point& point, orientation_2d orient) {
				return (orient == HORIZONTAL) ? point.a : point.b;
			}
		};
	}  // polygon
}  // boost

int main() {

	std::vector<Point> points;
	points.push_back(Point(0.5, 0));
	points.push_back(Point(1.5, 6));
	points.push_back(Point(2.5, 6));

	voronoi_diagram<double> vd;
	construct_voronoi(points.begin(), points.end(), &vd);

	return 0;
}