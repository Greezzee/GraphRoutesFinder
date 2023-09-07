#include "RouteSearchableGraph.h"
#include <stack>

void RouteSearchableGraph::clearSearchingInfo() {
	for (auto& node : m_Nodes) {
		node.second->searchingInfo.shortestRoutes = GraphRoutesToNode();
		node.second->searchingInfo.shortestRoutes.node = node.second->ID;
		node.second->searchingInfo.isVisited = false;
	}
}

GraphRoutesToNode RouteSearchableGraph::findShortestRoutes(NodeType startNodesType, GraphNodeID finishNode) {
	return findShortestRoutes(startNodesType, std::vector<GraphNodeID>({ finishNode }))[0];
}

std::vector<GraphRoutesToNode> RouteSearchableGraph::findShortestRoutes(NodeType startNodesType, const std::vector<GraphNodeID>& finishNode) {
	clearSearchingInfo();

	auto startNodesTypeIt = m_typedNodes.find(startNodesType);
	if (startNodesTypeIt != m_typedNodes.end()) {
		Dijkstra(startNodesType, WeightType(), true);
	}
	std::vector<GraphRoutesToNode> output;
	for (auto& node : finishNode) {
		output.push_back(m_Nodes[node]->searchingInfo.shortestRoutes);
	}
	return output;
}
std::vector<GraphRoutesToNode> RouteSearchableGraph::findShortestRoutes(NodeType startNodesType, NodeType finishNodesType) {
	clearSearchingInfo();

	auto startNodesTypeIt = m_typedNodes.find(startNodesType);
	auto finishNodesTypeIt = m_typedNodes.find(finishNodesType);
	if (startNodesTypeIt != m_typedNodes.end() && finishNodesTypeIt != m_typedNodes.end()) {
		Dijkstra(startNodesType, WeightType(), true);
	}
	std::vector<GraphRoutesToNode> output;
	for (auto& node : finishNodesTypeIt->second) {
		output.push_back(node->searchingInfo.shortestRoutes);
	}
	return output;
}
GraphRoutesToNode RouteSearchableGraph::findShortestRoutes(NodeType startNodesType, GraphNodeID finishNode, WeightType maxWeight) {
	return findShortestRoutes(startNodesType, std::vector<GraphNodeID>({ finishNode }), maxWeight)[0];
}
std::vector<GraphRoutesToNode> RouteSearchableGraph::findShortestRoutes(NodeType startNodesType, const std::vector<GraphNodeID>& finishNode, WeightType maxWeight) {
	clearSearchingInfo();

	auto startNodesTypeIt = m_typedNodes.find(startNodesType);
	if (startNodesTypeIt != m_typedNodes.end()) {
		Dijkstra(startNodesType, maxWeight, false);
	}
	std::vector<GraphRoutesToNode> output;
	for (auto& node : finishNode) {
		output.push_back(m_Nodes[node]->searchingInfo.shortestRoutes);
	}
	return output;
}
std::vector<GraphRoutesToNode> RouteSearchableGraph::findShortestRoutes(NodeType startNodesType, NodeType finishNodesType, WeightType maxWeight) {
	clearSearchingInfo();

	auto startNodesTypeIt = m_typedNodes.find(startNodesType);
	auto finishNodesTypeIt = m_typedNodes.find(finishNodesType);
	if (startNodesTypeIt != m_typedNodes.end() && finishNodesTypeIt != m_typedNodes.end()) {
		Dijkstra(startNodesType, maxWeight, false);
	}
	std::vector<GraphRoutesToNode> output;
	for (auto& node : finishNodesTypeIt->second) {
		output.push_back(node->searchingInfo.shortestRoutes);
	}
	return output;
}

void RouteSearchableGraph::Dijkstra(NodeType startNodesType, WeightType maxWeight, bool findOverweightRoutes) {
	auto cmp = [](node_ptr a, node_ptr b) {
		if (a->searchingInfo.shortestRoutes.isRoutesExist && b->searchingInfo.shortestRoutes.isRoutesExist)
			return a->searchingInfo.shortestRoutes.routeWeight < b->searchingInfo.shortestRoutes.routeWeight;
		if (a->searchingInfo.shortestRoutes.isRoutesExist)
			return true;
		return a < b;
		};

	std::set<node_ptr, decltype(cmp)> nodesToVisit(cmp);

	for (auto& start : m_typedNodes[startNodesType]) {

		nodesToVisit.insert(start);

		start->searchingInfo.shortestRoutes.node = start->ID;
		start->searchingInfo.shortestRoutes.isRoutesExist = true;
		start->searchingInfo.shortestRoutes.routeWeight = WeightType();
		start->searchingInfo.shortestRoutes.routesData = { GraphRoute() };
		start->searchingInfo.shortestRoutes.routesData.back().start = start->ID;
		start->searchingInfo.shortestRoutes.routesData.back().finish = start->ID;
		start->searchingInfo.shortestRoutes.routesData.back().routeWeight = WeightType();
		start->searchingInfo.shortestRoutes.routesData.back().nodeRoute = { start->ID };
	}

	while (!nodesToVisit.empty()) {
		auto curNode = *(nodesToVisit.begin());
		nodesToVisit.erase(nodesToVisit.begin());
		curNode->searchingInfo.isVisited = true;

		for (auto& link : curNode->outputLinks) {
			auto castedLink = m_caster.castLink(link);
			auto nextNode = m_caster.castNode(link->to);
			if (!link->isDirected && nextNode == curNode) // processing undirected links
				nextNode = m_caster.castNode(link->from);

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

	for (auto& node_pair : m_Nodes)
		node_pair.second->searchingInfo.isVisited = false;
}