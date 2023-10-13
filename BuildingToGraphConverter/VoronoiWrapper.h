#pragma once
#include <vector>
#include "Vector2D.h"
#define JC_VORONOI_IMPLEMENTATION
#include "jc_voronoi/jc_voronoi.h"
#define JCV_REAL_TYPE double
#define JCV_FABS fabs
#define JCV_ATAN2 atan2
#define JCV_CEIL ceil
#define JCV_FLOOR floor
#define JCV_FLT_MAX 1.7976931348623157E+308

namespace voronoi {

template <typename point_t>
struct Edge {
    Segment2D<point_t> edge;
    int neighbor_index = -1;
};

template <typename central_point_t, typename edge_point_t = central_point_t>
struct Site {
    central_point_t center;
    std::vector<Edge<edge_point_t>> edges;
};

// Template parameter should cast to double
template <typename central_point_t, typename edge_point_t = central_point_t>
class VoronoiWrapper final {

public:
	VoronoiWrapper() : 
        m_bounding_box({ { 0.0, 0.0 }, { 1.0, 1.0 } }) {
        memset(&m_diagram, 0, sizeof(jcv_diagram));
    }

    void setBoundingBox(const edge_point_t& left_down, const edge_point_t& up_right) {
        m_bounding_box = { { left_down.x, left_down.y }, { up_right.x, up_right.y } };
    }

    void setPoints(const std::vector<central_point_t>& points) {
        m_full_points_data = points;
        m_points.clear();
        for (auto i : points)
            m_points.push_back({ static_cast<jcv_real>(i.x), static_cast<jcv_real>(i.y) });
    }

    std::vector<Site<edge_point_t>> constructVoronoi() {
        memset(&m_diagram, 0, sizeof(jcv_diagram));

        jcv_diagram_generate(m_points.size(), (const jcv_point*)(&m_points[0]), &m_bounding_box, 0, &m_diagram);
        auto jcv_sites = jcv_diagram_get_sites(&m_diagram);

        std::vector<Site<edge_point_t>> m_sites(m_diagram.numsites);

        for (size_t i = 0; i < m_diagram.numsites; i++) {

            auto& site = jcv_sites[i];

            m_sites[site.index].center = m_full_points_data[site.index];

            auto graph_edge = jcv_sites[i].edges;
            while (graph_edge) {

                m_sites[site.index].edges.push_back(Edge<edge_point_t>{});

                m_sites[site.index].edges.back().edge.offset.x = (double)graph_edge->pos[0].x;
                m_sites[site.index].edges.back().edge.offset.y = (double)graph_edge->pos[0].y;
                m_sites[site.index].edges.back().edge.x = (double)graph_edge->pos[1].x - (double)graph_edge->pos[0].x;
                m_sites[site.index].edges.back().edge.y = (double)graph_edge->pos[1].y - (double)graph_edge->pos[0].y;
                m_sites[site.index].edges.back().neighbor_index = -1;
                if (graph_edge->neighbor)
                    m_sites[site.index].edges.back().neighbor_index = graph_edge->neighbor->index;

                graph_edge = graph_edge->next;
            }
        }
        jcv_diagram_free(&m_diagram);
        return m_sites;
    }



private:

    jcv_rect m_bounding_box;
    jcv_diagram m_diagram;
    std::vector<central_point_t> m_full_points_data;
    std::vector<jcv_point> m_points;
};

}