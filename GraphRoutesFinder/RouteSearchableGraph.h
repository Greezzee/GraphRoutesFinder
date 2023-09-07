#pragma once
#include <memory>
#include <vector>
#include <set>
#include <map>

#include "TypedGraph.h"
#include "WeightedGraph.h"
#include "NodeType.h"

// Type for link's weight in weighted graph
using WeightType = int;

// Describes one route from start to finish
struct GraphRoute 
{
	GraphNodeID start = UnsetGraphNodeID, finish = UnsetGraphNodeID;
	WeightType routeWeight = WeightType();

	std::vector<GraphNodeID> nodeRoute;
	std::vector<GraphLinkID> linksRoute;
};

// Describes all found routes to node
struct GraphRoutesToNode
{
	GraphNodeID node = UnsetGraphNodeID;
	bool isRoutesExist = false;
	WeightType routeWeight = WeightType();
	std::vector<GraphRoute> routesData;
};

namespace detail {
	struct GraphNodeWithRoutesData : public GraphTypedNode {
		struct searchingNodeInfo {
			GraphRoutesToNode shortestRoutes;
			bool isVisited = false;
		} searchingInfo;
	};
}

class RouteSearchableGraph final : 
	public TypedGraph<detail::GraphNodeWithRoutesData, detail::GraphWeightedLink<WeightType>>, 
	public WeightedGraph<WeightType, detail::GraphNodeWithRoutesData, detail::GraphWeightedLink<WeightType>>
{
public:
	RouteSearchableGraph() { clearSearchingInfo(); };
	~RouteSearchableGraph() = default;

	// returns array of all shortest paths
	GraphRoutesToNode findShortestRoutes(NodeType startNodesType, GraphNodeID finishNode);
	// array for every finish node of array of all shortest paths
	std::vector<GraphRoutesToNode> findShortestRoutes(NodeType startNodesType, const std::vector<GraphNodeID>& finishNode);
	// array for every finish node of array of all shortest paths
	std::vector<GraphRoutesToNode> findShortestRoutes(NodeType startNodesType, NodeType finishNodesType);
	// returns array of all shortest paths. Includes only routes with weight less than maxWeight
	GraphRoutesToNode findShortestRoutes(NodeType startNodesType, GraphNodeID finishNode, WeightType maxWeight);
	// array for every finish node of array of all shortest paths. Includes only routes with weight less than maxWeight
	std::vector<GraphRoutesToNode> findShortestRoutes(NodeType startNodesType, const std::vector<GraphNodeID>& finishNode, WeightType maxWeight);
	// array for every finish node of array of all shortest paths. Includes only routes with weight less than maxWeight
	std::vector<GraphRoutesToNode> findShortestRoutes(NodeType startNodesType, NodeType finishNodesType, WeightType maxWeight);
	// array of nodes that can't be reachable from any node of type
	std::vector<GraphNodeID> getUnreachableNodes(NodeType startType);
private:
	using node_ptr = std::shared_ptr<detail::GraphNodeWithRoutesData>;
	using link_ptr = std::shared_ptr<detail::GraphWeightedLink<WeightType>>;
	using node_t = detail::GraphNodeWithRoutesData;
	using link_t = detail::GraphWeightedLink<WeightType>;

	void clearSearchingInfo();

	// starts in finish, find route from some node from nodesToStartIn to finish
	void Dijkstra(NodeType startNodesType, WeightType maxWeight, bool findOverweightRoutes);
};