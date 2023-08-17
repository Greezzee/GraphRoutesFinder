#include "RouteSearchableGraph.h"
#include <stack>

RouteSearchableGraph::RouteSearchableGraph() {
	m_nextFreeGraphLinkID = UnsetGraphLinkID;
	m_nextFreeGraphNodeID = UnsetGraphNodeID;
	m_nextFreeGraphLinkID++;
	m_nextFreeGraphNodeID++;
}

GraphNodeID RouteSearchableGraph::createNode(NodeType type) {
	auto newNode = node_ptr(new GraphNode);

	newNode->ID = m_nextFreeGraphNodeID;
	newNode->type = type;
	m_Nodes[m_nextFreeGraphNodeID] = newNode;
	m_typedNodes[type].insert(newNode);

	m_nextFreeGraphNodeID++;

	return newNode->ID;
}

GraphLinkID RouteSearchableGraph::createLink(GraphNodeID from, GraphNodeID to, WeightType weight, bool isDirected) {
	auto fromIt = m_Nodes.find(from);
	auto toIt = m_Nodes.find(to);

	if (fromIt == m_Nodes.end() || toIt == m_Nodes.end())
		return UnsetGraphLinkID;

	auto newLink = link_ptr(new GraphLink);
	newLink->ID = m_nextFreeGraphLinkID;
	newLink->from = fromIt->second;
	newLink->to = toIt->second;
	newLink->isDirected = isDirected;
	newLink->weight = weight;

	m_Links[m_nextFreeGraphLinkID] = newLink;
	m_nextFreeGraphLinkID++;

	toIt->second->inputLinks.insert(newLink);
	fromIt->second->outputLinks.insert(newLink);

	if (!isDirected) {
		toIt->second->outputLinks.insert(newLink);
		fromIt->second->inputLinks.insert(newLink);
	}

	return newLink->ID;
}

void RouteSearchableGraph::removeNode(GraphNodeID node) {
	auto nodeIt = m_Nodes.find(node);
	if (nodeIt == m_Nodes.end())
		return;

	node_ptr removingNode = nodeIt->second;
	m_Nodes.erase(nodeIt);

	auto typedNodesIt = m_typedNodes.find(removingNode->type);
	if (typedNodesIt != m_typedNodes.end())
		typedNodesIt->second.erase(removingNode);
	std::set<link_ptr> linksToRemove = removingNode->inputLinks;
	linksToRemove.insert(removingNode->outputLinks.begin(), removingNode->outputLinks.end());

	for (auto link : linksToRemove)
		removeLink(link);
}

void RouteSearchableGraph::removeLink(GraphLinkID link) {
	auto linkIt = m_Links.find(link);
	if (linkIt == m_Links.end())
		return;

	link_ptr removingLink = linkIt->second;
	m_Links.erase(linkIt);

	removeLink(removingLink);
}

void RouteSearchableGraph::removeLink(link_ptr link) {
	link->to->inputLinks.erase(link);
	link->from->outputLinks.erase(link);

	if (!link->isDirected) {
		link->to->outputLinks.erase(link);
		link->from->inputLinks.erase(link);
	}

	link->to.reset();
	link->from.reset();
}

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
		for (auto& startNode : m_typedNodes[startNodesType])
			Dijkstra(startNode, WeightType(), true);
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
		for (auto& startNode : m_typedNodes[startNodesType])
			Dijkstra(startNode, WeightType(), true);
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
		for (auto& startNode : m_typedNodes[startNodesType])
			Dijkstra(startNode, maxWeight, false);
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
		for (auto& startNode : m_typedNodes[startNodesType])
			Dijkstra(startNode, maxWeight, false);
	}
	std::vector<GraphRoutesToNode> output;
	for (auto& node : finishNodesTypeIt->second) {
		output.push_back(node->searchingInfo.shortestRoutes);
	}
	return output;
}

void RouteSearchableGraph::forwardDFS(node_ptr start, WeightType maxWeight, bool findOverweightRoutes) {

	std::stack<node_ptr> stackRoute;
	stackRoute.push(start);

	start->searchingInfo.isVisited = true;
	start->searchingInfo.shortestRoutes.node = start->ID;
	start->searchingInfo.shortestRoutes.isRoutesExist = true;
	start->searchingInfo.shortestRoutes.routeWeight = WeightType();
	start->searchingInfo.shortestRoutes.routesData = { GraphRoute() };
	start->searchingInfo.shortestRoutes.routesData.back().start = start->ID;
	start->searchingInfo.shortestRoutes.routesData.back().finish = start->ID;
	start->searchingInfo.shortestRoutes.routesData.back().routeWeight = WeightType();
	start->searchingInfo.shortestRoutes.routesData.back().nodeRoute = { start->ID };

	while (!stackRoute.empty()) {
		auto curNode = stackRoute.top();
		stackRoute.pop();

		for (auto& outputLink : curNode->outputLinks) {
			auto nextNode = outputLink->to;
			if (!outputLink->isDirected && nextNode == curNode) // processing undirected links
				nextNode = outputLink->from;

			auto& curNodeRoutes = curNode->searchingInfo.shortestRoutes;
			auto& nextNodeRoutes = nextNode->searchingInfo.shortestRoutes;

			if (nextNodeRoutes.isRoutesExist &&
				curNodeRoutes.routeWeight + outputLink->weight > nextNodeRoutes.routeWeight)
				continue;
			
			if (!findOverweightRoutes && curNodeRoutes.routeWeight + outputLink->weight > maxWeight)
				continue;

			stackRoute.push(nextNode);

			if (!nextNodeRoutes.isRoutesExist || nextNodeRoutes.routeWeight > curNodeRoutes.routeWeight + outputLink->weight) {
				nextNodeRoutes.routeWeight = curNodeRoutes.routeWeight + outputLink->weight;
				nextNodeRoutes.routesData.clear();
			}

			nextNode->searchingInfo.isVisited = true;
			nextNodeRoutes.isRoutesExist = true;
			nextNodeRoutes.node = nextNode->ID;

			for (auto& route : curNodeRoutes.routesData) {
				nextNodeRoutes.routesData.push_back(route);
				nextNodeRoutes.routesData.back().nodeRoute.push_back(nextNode->ID);
				nextNodeRoutes.routesData.back().linksRoute.push_back(outputLink->ID);
				nextNodeRoutes.routesData.back().finish = nextNode->ID;
				nextNodeRoutes.routesData.back().routeWeight += outputLink->weight;
			}
		}
	}

	for (auto& node_pair : m_Nodes)
		node_pair.second->searchingInfo.isVisited = false;
}

void RouteSearchableGraph::Dijkstra(node_ptr start, WeightType maxWeight, bool findOverweightRoutes) {
	auto cmp = [](node_ptr a, node_ptr b) {
		if (a->searchingInfo.shortestRoutes.isRoutesExist && b->searchingInfo.shortestRoutes.isRoutesExist)
			return a->searchingInfo.shortestRoutes.routeWeight < b->searchingInfo.shortestRoutes.routeWeight;
		if (a->searchingInfo.shortestRoutes.isRoutesExist)
			return true;
		return a < b;
		};

	std::set<node_ptr, decltype(cmp)> nodesToVisit(cmp);
	nodesToVisit.insert(start);

	start->searchingInfo.shortestRoutes.node = start->ID;
	start->searchingInfo.shortestRoutes.isRoutesExist = true;
	start->searchingInfo.shortestRoutes.routeWeight = WeightType();
	start->searchingInfo.shortestRoutes.routesData = { GraphRoute() };
	start->searchingInfo.shortestRoutes.routesData.back().start = start->ID;
	start->searchingInfo.shortestRoutes.routesData.back().finish = start->ID;
	start->searchingInfo.shortestRoutes.routesData.back().routeWeight = WeightType();
	start->searchingInfo.shortestRoutes.routesData.back().nodeRoute = { start->ID };

	while (!nodesToVisit.empty()) {
		auto curNode = *(nodesToVisit.begin());
		nodesToVisit.erase(nodesToVisit.begin());
		curNode->searchingInfo.isVisited = true;

		for (auto& link : curNode->outputLinks) {
			auto nextNode = link->to;
			if (!link->isDirected && nextNode == curNode) // processing undirected links
				nextNode = link->from;

			if (nextNode->searchingInfo.isVisited)
				continue;

			auto& curNodeRoutes = curNode->searchingInfo.shortestRoutes;
			auto& nextNodeRoutes = nextNode->searchingInfo.shortestRoutes;

			if (nextNodeRoutes.isRoutesExist &&
				curNodeRoutes.routeWeight + link->weight > nextNodeRoutes.routeWeight)
				continue;

			if (!findOverweightRoutes && curNodeRoutes.routeWeight + link->weight > maxWeight)
				continue;

			if (!nextNodeRoutes.isRoutesExist || nextNodeRoutes.routeWeight > curNodeRoutes.routeWeight + link->weight) {
				nextNodeRoutes.routeWeight = curNodeRoutes.routeWeight + link->weight;
				nextNodeRoutes.routesData.clear();
			}

			nextNodeRoutes.isRoutesExist = true;
			nextNodeRoutes.node = nextNode->ID;

			for (auto& route : curNodeRoutes.routesData) {
				nextNodeRoutes.routesData.push_back(route);
				nextNodeRoutes.routesData.back().nodeRoute.push_back(nextNode->ID);
				nextNodeRoutes.routesData.back().linksRoute.push_back(link->ID);
				nextNodeRoutes.routesData.back().finish = nextNode->ID;
				nextNodeRoutes.routesData.back().routeWeight += link->weight;
			}

			nodesToVisit.insert(nextNode);
		}
	}

	for (auto& node_pair : m_Nodes)
		node_pair.second->searchingInfo.isVisited = false;
}

bool RouteSearchableGraph::isNodeOfTypeExists(NodeType type) {
	auto nodesTypeIt = m_typedNodes.find(type);
	if (nodesTypeIt == m_typedNodes.end()) 
		return false;
	return !nodesTypeIt->second.empty();
}

bool RouteSearchableGraph::isNodeExists(GraphNodeID node) {
	return m_Nodes.find(node) != m_Nodes.end();
}
bool RouteSearchableGraph::isLinkExists(GraphLinkID link) {
	return m_Links.find(link) != m_Links.end();
}
bool RouteSearchableGraph::isLinkExists(GraphNodeID from, GraphNodeID to) {
	auto fromIt = m_Nodes.find(from);
	if (fromIt == m_Nodes.end())
		return false;
	for (auto& link : fromIt->second->outputLinks)
		if (link->to->ID == to)
			return true;
	return false;
}

std::vector<GraphNodeID> RouteSearchableGraph::getExternalNodes() {
	std::vector<GraphNodeID> out;
	for (auto& node : m_Nodes)
		if (node.second->inputLinks.empty() && node.second->outputLinks.empty())
			out.push_back(node.second->ID);
	return out;
}

bool RouteSearchableGraph::isConnected() {
	clearSearchingInfo();

	std::stack<node_ptr> stackRoute;
	stackRoute.push(m_Nodes.begin()->second);

	while (!stackRoute.empty()) { // using DFS to check connectivity 
		node_ptr node = stackRoute.top();
		stackRoute.pop();
		node->searchingInfo.isVisited = true;

		for (auto& outputLink : node->outputLinks)
			if (!outputLink->to->searchingInfo.isVisited)
				stackRoute.push(outputLink->to);
		for (auto& inputLink : node->inputLinks)
			if (!inputLink->from->searchingInfo.isVisited)
				stackRoute.push(inputLink->from);
	}

	for (auto& node : m_Nodes) // all nodes should be marked as visited
		if (!node.second->searchingInfo.isVisited)
			return false;
	return true;
}