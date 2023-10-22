#pragma once
#include <unordered_map>
#include <set>
#include <functional>
#include "Vector2D.h"
#include "WallsArray.h"

namespace voronoi {

using SiteID = int;
const SiteID UnsetSiteID = -1;

template <typename central_point_t, typename edge_point_t = central_point_t>
struct Edge : public Segment2D<edge_point_t> {
    SiteID neighborID;
};

template <typename central_point_t, typename edge_point_t = central_point_t>
struct Site {

    using edgeCoordType = typename edge_point_t::CoordType;

    std::vector<Edge<central_point_t, edge_point_t>> edges;
    central_point_t center;

    SiteID myID;

    // area using Shoelace formula
    edgeCoordType getArea() {
        edgeCoordType area = 0;

        for (size_t i = 0, sz = edges.size(); i < sz; ++i) {
            area += edges[i].offset.x * edges[(i + 1) % sz].offset.y -
                edges[i].offset.y * edges[(i + 1) % sz].offset.x;
        }

        return std::abs(area / 2.);
    }
};

namespace detail {

    using EdgeBorder = std::vector<std::pair<size_t, size_t>>;

    template <typename T>
    EdgeBorder getBorderWithProperty(const std::vector<T>& testArr, std::function<bool(const T&)> testProperty) {
        EdgeBorder out;

        if (testArr.empty())
            return out;

        size_t start = 1;
        for (size_t end = testArr.size(); start < end; ++start) {
            if (testProperty(testArr[start]) && !testProperty(testArr[start - 1])) {
                break;
            }
        }

        if (start == testArr.size()) {
            if (testProperty(testArr[0]))
                out.push_back({ 0, start });
            return out;
        }

        for (size_t i = 0, end = testArr.size(); i < end; ++i) {

            size_t a = (i + start) % end;
            size_t b = (i + start - 1) % end;

            if (testProperty(testArr[a]) && !testProperty(testArr[b])) {
                out.push_back({ i + start, 0 });
            }
            else if (!testProperty(testArr[a]) && testProperty(testArr[b])) {
                out.back().second = i + start - out.back().first;
            }
        }

        return out;
    }

} // namespace detail

// List of voronoi sites
template <typename central_point_t, typename edge_point_t = central_point_t>
class VoronoiDiagram final : public std::unordered_map<SiteID, Site<central_point_t, edge_point_t>>
{
    using coordType = typename Site<central_point_t, edge_point_t>::edgeCoordType;
    using edgeType = Edge<central_point_t, edge_point_t>;
    using siteType = Site<central_point_t, edge_point_t>;

public:
    void removeSitesInsideWalls(const WallsArray<typename central_point_t::CoordType>& walls) {

        for (auto first = this->begin(), last = this->end(); first != last;)
        {
            auto& curSite = (*first).second;
            if (walls.isPointInsideWall(curSite.center)) {

                for (auto& edge : curSite.edges) {
                    auto& site = (*this)[edge.neighborID];
                    for (auto& otherSiteEdge : site.edges) if (otherSiteEdge.neighborID == (*first).first)
                        otherSiteEdge.neighborID = UnsetSiteID;
                }

                first = this->erase(first);
            }
            else
                ++first;
        }

    }

    void mergeSitesToArea(const coordType& targetArea) {
        for (auto first = this->begin(); first != this->end();) {
            auto& id = (*first).first;
            auto& site = (*first).second;

            while (site.getArea() < targetArea) {

                int minAreaNeighborIndex = -1;
                coordType minAreaNeighbor = 0;

                for (int i = 0, sz = site.edges.size(); i < sz; ++i) {
                    auto neighborID = site.edges[i].neighborID;
                    if (neighborID == UnsetSiteID)
                        continue;

                    auto area = (*this)[neighborID].getArea();

                    if (minAreaNeighbor == 0 || area < minAreaNeighbor) {
                        minAreaNeighbor = area;
                        minAreaNeighborIndex = i;
                    }
                }

                if (minAreaNeighborIndex == -1)
                    break;

                auto& connectingSite = (*this)[site.edges[minAreaNeighborIndex].neighborID];
                auto connectingSiteID = site.edges[minAreaNeighborIndex].neighborID;

                site.edges.erase(site.edges.begin() + minAreaNeighborIndex);
                site.edges.insert(site.edges.begin() + minAreaNeighborIndex, connectingSite.edges.begin(), connectingSite.edges.end());

                std::remove_if(site.edges.begin(), site.edges.end(), [&](const edgeType& x) { return x.neighborID == id; });

                for (auto& edge : connectingSite.edges) if (edge.neighborID != UnsetSiteID) {
                    for (auto& otherEdge : (*this)[edge.neighborID].edges) if (otherEdge.neighborID != UnsetSiteID) {
                        auto& otherSite = (*this)[otherEdge.neighborID];
                        for (auto& otherSiteEdge : otherSite.edges) if (otherSiteEdge.neighborID == connectingSiteID)
                            otherSiteEdge.neighborID = id;
                    }
                }

                this->erase(connectingSiteID);
            }

            ++first;
        }
    }

private:

    siteType mergeSites(const siteType& a, const siteType& b) {
        auto abBorder = detail::getBorderWithProperty(a.edges, [&](const edgeType& x) { return x.neighborID == b.myID; });
        auto baBorder = detail::getBorderWithProperty(b.edges, [&](const edgeType& x) { return x.neighborID == a.myID; });

        siteType newSite;
        newSite.myID = UnsetSiteID;

        if (abBorder.size() != 1 || baBorder.size() != 1)
            return newSite;


    }

};

} // namespace voronoi