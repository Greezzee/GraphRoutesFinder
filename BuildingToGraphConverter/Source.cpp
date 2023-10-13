#include "VoronoiWrapper.h"

#define NPOINT 10

//int main() {
//
//    std::vector<voronoi::Vector2D<double>> points(NPOINT);
//
//    for (size_t i = 0; i < NPOINT; i++) {
//        points[i].x = ((float)rand() / (1.0f + (float)RAND_MAX));
//        points[i].y = ((float)rand() / (1.0f + (float)RAND_MAX));
//    }
//
//    printf("# Seed sites\n");
//    for (size_t i = 0; i < NPOINT; i++) {
//        printf("%f %f\n", (double)points[i].x, (double)points[i].y);
//    }
//
//    voronoi::VoronoiWrapper<voronoi::Vector2D<double>> wrapper;
//    wrapper.setPoints(points);
//    auto output = wrapper.constructVoronoi();
//
//    return 0;
//}