#pragma once
#include <memory>
#include <vector>
#include <set>
#include <map>

#include "TypedGraph.h"
#include "WeightedGraph.h"
#include "NodeType.h"

namespace graphs {

// Describes one route from start to finish
template <typename LinkWeight_t>
struct GraphRoute 
{
	GraphNodeID start = UnsetGraphNodeID, finish = UnsetGraphNodeID;
	LinkWeight_t routeWeight = LinkWeight_t();

	std::vector<GraphNodeID> nodeRoute;
	std::vector<GraphLinkID> linksRoute;
};

// Describes all found routes to node
template <typename LinkWeight_t>
struct GraphRoutesToNode
{
	GraphNodeID node = UnsetGraphNodeID;
	bool isRoutesExist = false;
	LinkWeight_t routeWeight = LinkWeight_t();
	std::vector<GraphRoute<LinkWeight_t>> routesData;
};

namespace detail {
	template <typename NodeType_t, typename LinkWeight_t>
	struct GraphNodeWithRoutesData : public GraphTypedNode<typename NodeType_t>{
		struct searchingNodeInfo {
			GraphRoutesToNode<LinkWeight_t> shortestRoutes;
			bool isVisited = false;
		} searchingInfo;
	};
}

namespace detail {

template <typename Node_t, typename Link_t>
class RouteSearchableGraphWithCustomStructure : 
	public TypedGraph<Node_t, Link_t>,
	public WeightedGraph<Node_t, Link_t>
{

	using NodeType_t = typename Node_t::node_type_t;
	using LinkWeight_t = typename Link_t::link_weight_t;

public:
	RouteSearchableGraphWithCustomStructure() { clearSearchingInfo(); };
	virtual ~RouteSearchableGraphWithCustomStructure() = default;

	// returns array of all shortest paths
	GraphRoutesToNode<LinkWeight_t> findShortestRoutes(NodeType_t startNodesType, GraphNodeID finishNode);
	// array for every finish node of array of all shortest paths
	std::vector<GraphRoutesToNode<LinkWeight_t>> findShortestRoutes(NodeType_t startNodesType, const std::vector<GraphNodeID>& finishNode);
	// array for every finish node of array of all shortest paths
	std::vector<GraphRoutesToNode<LinkWeight_t>> findShortestRoutes(NodeType_t startNodesType, NodeType_t finishNodesType);
	// returns array of all shortest paths. Includes only routes with weight less than maxWeight
	GraphRoutesToNode<LinkWeight_t> findShortestRoutes(NodeType_t startNodesType, GraphNodeID finishNode, LinkWeight_t maxWeight);
	// array for every finish node of array of all shortest paths. Includes only routes with weight less than maxWeight
	std::vector<GraphRoutesToNode<LinkWeight_t>> findShortestRoutes(NodeType_t startNodesType, const std::vector<GraphNodeID>& finishNode, LinkWeight_t maxWeight);
	// array for every finish node of array of all shortest paths. Includes only routes with weight less than maxWeight
	std::vector<GraphRoutesToNode<LinkWeight_t>> findShortestRoutes(NodeType_t startNodesType, NodeType_t finishNodesType, LinkWeight_t maxWeight);
private:

	using node_t = detail::GraphNodeWithRoutesData<NodeType_t, LinkWeight_t>;
	using link_t = detail::GraphWeightedLink<LinkWeight_t>;

	using node_ptr = std::shared_ptr<node_t>;
	using link_ptr = std::shared_ptr<link_t>;

	void clearSearchingInfo();

	// starts in finish, find route from some node from nodesToStartIn to finish
	void Dijkstra(NodeType_t startNodesType, LinkWeight_t maxWeight, bool findOverweightRoutes);
};

template <typename Node_t, typename Link_t>
void RouteSearchableGraphWithCustomStructure<Node_t, Link_t>::clearSearchingInfo() {
	for (auto& node : this->m_Nodes) {
		node.second->searchingInfo.shortestRoutes = GraphRoutesToNode<LinkWeight_t>();
		node.second->searchingInfo.shortestRoutes.node = node.second->ID;
		node.second->searchingInfo.isVisited = false;
	}
}

template <typename Node_t, typename Link_t>
GraphRoutesToNode<typename RouteSearchableGraphWithCustomStructure<Node_t, Link_t>::LinkWeight_t> 
	RouteSearchableGraphWithCustomStructure<Node_t, Link_t>::findShortestRoutes(NodeType_t startNodesType, GraphNodeID finishNode) {
	return findShortestRoutes(startNodesType, std::vector<GraphNodeID>({ finishNode }))[0];
}

template <typename Node_t, typename Link_t>
std::vector<GraphRoutesToNode<typename RouteSearchableGraphWithCustomStructure<Node_t, Link_t>::LinkWeight_t>> 
	RouteSearchableGraphWithCustomStructure<Node_t, Link_t>::findShortestRoutes(NodeType_t startNodesType, const std::vector<GraphNodeID>& finishNode) {
	clearSearchingInfo();

	auto startNodesTypeIt = this->m_typedNodes.find(startNodesType);
	if (startNodesTypeIt != this->m_typedNodes.end()) {
		Dijkstra(startNodesType, LinkWeight_t(), true);
	}
	std::vector<GraphRoutesToNode> output;
	for (auto& node : finishNode) {
		output.push_back(this->m_Nodes[node]->searchingInfo.shortestRoutes);
	}
	return output;
}

template <typename Node_t, typename Link_t>
std::vector<GraphRoutesToNode<typename RouteSearchableGraphWithCustomStructure<Node_t, Link_t>::LinkWeight_t>> 
	RouteSearchableGraphWithCustomStructure<Node_t, Link_t>::findShortestRoutes(NodeType_t startNodesType, NodeType_t finishNodesType) {
	clearSearchingInfo();

	auto startNodesTypeIt = this->m_typedNodes.find(startNodesType);
	auto finishNodesTypeIt = this->m_typedNodes.find(finishNodesType);
	if (startNodesTypeIt != this->m_typedNodes.end() && finishNodesTypeIt != this->m_typedNodes.end()) {
		Dijkstra(startNodesType, LinkWeight_t(), true);
	}
	std::vector<GraphRoutesToNode<LinkWeight_t>> output;
	for (auto& node : finishNodesTypeIt->second) {
		output.push_back(node->searchingInfo.shortestRoutes);
	}
	return output;
}

template <typename Node_t, typename Link_t>
GraphRoutesToNode<typename RouteSearchableGraphWithCustomStructure<Node_t, Link_t>::LinkWeight_t> 
	RouteSearchableGraphWithCustomStructure<Node_t, Link_t>::findShortestRoutes(NodeType_t startNodesType, GraphNodeID finishNode, LinkWeight_t maxWeight) {
	return findShortestRoutes(startNodesType, std::vector<GraphNodeID>({ finishNode }), maxWeight)[0];
}

template <typename Node_t, typename Link_t>
std::vector<GraphRoutesToNode<typename RouteSearchableGraphWithCustomStructure<Node_t, Link_t>::LinkWeight_t>> 
	RouteSearchableGraphWithCustomStructure<Node_t, Link_t>::findShortestRoutes(NodeType_t startNodesType, const std::vector<GraphNodeID>& finishNode, LinkWeight_t maxWeight) {
	clearSearchingInfo();

	auto startNodesTypeIt = this->m_typedNodes.find(startNodesType);
	if (startNodesTypeIt != this->m_typedNodes.end()) {
		Dijkstra(startNodesType, maxWeight, false);
	}
	std::vector<GraphRoutesToNode<LinkWeight_t>> output;
	for (auto& node : finishNode) {
		output.push_back(this->m_Nodes[node]->searchingInfo.shortestRoutes);
	}
	return output;
}

template <typename Node_t, typename Link_t>
std::vector<GraphRoutesToNode<typename RouteSearchableGraphWithCustomStructure<Node_t, Link_t>::LinkWeight_t>> 
	RouteSearchableGraphWithCustomStructure<Node_t, Link_t>::findShortestRoutes(NodeType_t startNodesType, NodeType_t finishNodesType, LinkWeight_t maxWeight) {
	clearSearchingInfo();

	auto startNodesTypeIt = this->m_typedNodes.find(startNodesType);
	auto finishNodesTypeIt = this->m_typedNodes.find(finishNodesType);
	if (startNodesTypeIt != this->m_typedNodes.end() && finishNodesTypeIt != this->m_typedNodes.end()) {
		Dijkstra(startNodesType, maxWeight, false);
	}
	std::vector<GraphRoutesToNode<LinkWeight_t>> output;
	for (auto& node : finishNodesTypeIt->second) {
		output.push_back(node->searchingInfo.shortestRoutes);
	}
	return output;
}

template <typename Node_t, typename Link_t>
void RouteSearchableGraphWithCustomStructure<Node_t, Link_t>::Dijkstra(NodeType_t startNodesType, LinkWeight_t maxWeight, bool findOverweightRoutes) {
	auto cmp = [](node_ptr a, node_ptr b) {
		if (a->searchingInfo.shortestRoutes.isRoutesExist && b->searchingInfo.shortestRoutes.isRoutesExist)
			return a->searchingInfo.shortestRoutes.routeWeight < b->searchingInfo.shortestRoutes.routeWeight;
		if (a->searchingInfo.shortestRoutes.isRoutesExist)
			return true;
		return a < b;
		};

	std::multiset<node_ptr, decltype(cmp)> nodesToVisit(cmp);

	for (auto& start : this->m_typedNodes[startNodesType]) {

		nodesToVisit.insert(start);

		start->searchingInfo.shortestRoutes.node = start->ID;
		start->searchingInfo.shortestRoutes.isRoutesExist = true;
		start->searchingInfo.shortestRoutes.routeWeight = LinkWeight_t();
		start->searchingInfo.shortestRoutes.routesData = { GraphRoute<LinkWeight_t>() };
		start->searchingInfo.shortestRoutes.routesData.back().start = start->ID;
		start->searchingInfo.shortestRoutes.routesData.back().finish = start->ID;
		start->searchingInfo.shortestRoutes.routesData.back().routeWeight = LinkWeight_t();
		start->searchingInfo.shortestRoutes.routesData.back().nodeRoute = { start->ID };
	}

	while (!nodesToVisit.empty()) {
		auto curNode = *(nodesToVisit.begin());
		nodesToVisit.erase(nodesToVisit.begin());

		if (curNode->searchingInfo.isVisited)
			continue;

		curNode->searchingInfo.isVisited = true;

		for (auto& link : curNode->outputLinks) {
			auto castedLink = this->m_caster.castLink(link);
			auto nextNode = this->m_caster.castNode(link->to);
			if (!link->isDirected && nextNode == curNode) // processing undirected links
				nextNode = this->m_caster.castNode(link->from);

			if (nextNode->searchingInfo.isVisited)
				continue;

			auto& curNodeRoutes = curNode->searchingInfo.shortestRoutes;
			auto& nextNodeRoutes = nextNode->searchingInfo.shortestRoutes;

			if (nextNodeRoutes.isRoutesExist &&
				curNodeRoutes.routeWeight + castedLink->weight > nextNodeRoutes.routeWeight)
				continue;

			if (!findOverweightRoutes && curNodeRoutes.routeWeight + castedLink->weight > maxWeight)
				continue;

			if (!nextNodeRoutes.isRoutesExist || nextNodeRoutes.routeWeight > curNodeRoutes.routeWeight + castedLink->weight) {
				nextNodeRoutes.routeWeight = curNodeRoutes.routeWeight + castedLink->weight;
				nextNodeRoutes.routesData.clear();
			}

			nextNodeRoutes.isRoutesExist = true;
			nextNodeRoutes.node = nextNode->ID;

			for (auto& route : curNodeRoutes.routesData) {
				nextNodeRoutes.routesData.push_back(route);
				nextNodeRoutes.routesData.back().nodeRoute.push_back(nextNode->ID);
				nextNodeRoutes.routesData.back().linksRoute.push_back(link->ID);
				nextNodeRoutes.routesData.back().finish = nextNode->ID;
				nextNodeRoutes.routesData.back().routeWeight += castedLink->weight;
			}

			nodesToVisit.insert(nextNode);
		}
	}

	for (auto& node_pair : this->m_Nodes)
		node_pair.second->searchingInfo.isVisited = false;
}
} // namespace detail

template <typename LinkWeight_t = float, typename NodeType_t = NodeType>
using RouteSearchableGraph = detail::RouteSearchableGraphWithCustomStructure<detail::GraphNodeWithRoutesData<NodeType_t, LinkWeight_t>, detail::GraphWeightedLink<LinkWeight_t>>;

} // namespace graphs