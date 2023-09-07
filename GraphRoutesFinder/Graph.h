#pragma once

#include <memory>
#include <vector>
#include <set>
#include <map>
#include <stack>

using GraphNodeID = unsigned;
using GraphLinkID = unsigned;

const GraphNodeID UnsetGraphNodeID = GraphNodeID();
const GraphLinkID UnsetGraphLinkID = GraphLinkID();

// namespace for inner usage, you don't need anything from it to work
namespace detail {

	struct GraphLink;

	struct GraphNode
	{
		GraphNodeID ID = UnsetGraphNodeID;
		std::set<std::shared_ptr<GraphLink>> inputLinks, outputLinks;

		bool isVisited = false;

		virtual ~GraphNode() = default;
	};

	struct GraphLink
	{
		GraphLinkID ID = UnsetGraphLinkID;
		std::shared_ptr<GraphNode> from, to;
		bool isDirected = true;

		virtual ~GraphLink() = default;
	};

	template <typename Node_t, typename Link_t>
	struct GraphNodeLinkCaster 
	{
		inline std::shared_ptr<Node_t> castNode(const std::shared_ptr<GraphNode>& ptr) {
			return std::static_pointer_cast<Node_t>(ptr);
		}

		inline std::shared_ptr<Link_t> castLink(const std::shared_ptr<GraphLink>& ptr) {
			return std::static_pointer_cast<Link_t>(ptr);
		}
	};
}

template <typename Node_t = detail::GraphNode, typename Link_t = detail::GraphLink>
class Graph
{
public:
	Graph();
	virtual ~Graph() = default;

	// creates Node with type, returns its ID
	GraphNodeID createNode();
	// creates link between 2 nodes with some weight. Can be directed or undirected
	GraphLinkID createLink(GraphNodeID from, GraphNodeID to, bool isDirected = false);

	// remove node from graph. Also removes all links to/from this node
	void removeNode(GraphNodeID node);
	void removeLink(GraphLinkID link);
	
	bool isNodeExists(GraphNodeID node);
	bool isLinkExists(GraphLinkID link);
	bool isLinkExists(GraphNodeID from, GraphNodeID to);

	// returns array of nodes whitch doesn't have links to any other nodes
	std::vector<GraphNodeID> getExternalNodes();

	// returns true if graph is weakly connected (if all links are undirected it's equal to connected)
	bool isConnected();
	
protected:
	using node_ptr = std::shared_ptr<typename Node_t>;
	using link_ptr = std::shared_ptr<typename Link_t>;

	// Maps of all nodes and links of a graph
	std::map<GraphNodeID, node_ptr> m_Nodes;
	std::map<GraphLinkID, link_ptr> m_Links;

	detail::GraphNodeLinkCaster<typename Node_t, typename Link_t> m_caster;

	std::pair<GraphNodeID, node_ptr> createNewNode();
	std::pair<GraphLinkID, link_ptr> createNewLink(GraphNodeID from, GraphNodeID to, bool isDirected = false);

	void setNodesUnvisited();

	// starts in start, mark as visited all reachable nodes throw forward dfs
	void forwardDFS(node_ptr start);
private:
	void removeLink(link_ptr link);

	GraphNodeID m_nextFreeGraphNodeID;
	GraphLinkID m_nextFreeGraphLinkID;
};

template <typename Node_t, typename Link_t>
Graph<Node_t, Link_t>::Graph() {
	m_nextFreeGraphLinkID = UnsetGraphLinkID;
	m_nextFreeGraphNodeID = UnsetGraphNodeID;
	m_nextFreeGraphLinkID++;
	m_nextFreeGraphNodeID++;
}

template <typename Node_t, typename Link_t>
GraphNodeID Graph<Node_t, Link_t>::createNode() {
	return createNewNode().first;
}

template <typename Node_t, typename Link_t>
GraphLinkID Graph<Node_t, Link_t>::createLink(GraphNodeID from, GraphNodeID to, bool isDirected) {
	return createNewLink(from, to, isDirected).first;
}

template <typename Node_t, typename Link_t>
std::pair<GraphNodeID, typename Graph<Node_t, Link_t>::node_ptr> Graph<Node_t, Link_t>::createNewNode() {
	auto newNode = node_ptr(new Node_t);

	newNode->ID = m_nextFreeGraphNodeID;
	m_Nodes[m_nextFreeGraphNodeID] = newNode;
	auto out = std::pair<GraphLinkID, node_ptr>({ m_nextFreeGraphNodeID, newNode });
	m_nextFreeGraphNodeID++;

	return out;
}

template <typename Node_t, typename Link_t>
std::pair<GraphLinkID, typename Graph<Node_t, Link_t>::link_ptr> Graph<Node_t, Link_t>::createNewLink(GraphNodeID from, GraphNodeID to, bool isDirected) {
	auto fromIt = m_Nodes.find(from);
	auto toIt = m_Nodes.find(to);

	if (fromIt == m_Nodes.end() || toIt == m_Nodes.end())
		return {UnsetGraphLinkID, nullptr};

	auto newLink = link_ptr(new Link_t);
	newLink->ID = m_nextFreeGraphLinkID;
	newLink->from = fromIt->second;
	newLink->to = toIt->second;
	newLink->isDirected = isDirected;

	m_Links[m_nextFreeGraphLinkID] = newLink;
	auto out = std::pair<GraphLinkID, link_ptr>({m_nextFreeGraphLinkID, newLink });
	m_nextFreeGraphLinkID++;

	toIt->second->inputLinks.insert(newLink);
	fromIt->second->outputLinks.insert(newLink);

	if (!isDirected) {
		toIt->second->outputLinks.insert(newLink);
		fromIt->second->inputLinks.insert(newLink);
	}

	return out;
}

template <typename Node_t, typename Link_t>
void Graph<Node_t, Link_t>::removeNode(GraphNodeID node) {
	auto nodeIt = m_Nodes.find(node);
	if (nodeIt == m_Nodes.end())
		return;

	node_ptr removingNode = nodeIt->second;
	m_Nodes.erase(nodeIt);

	auto linksToRemove = removingNode->inputLinks;
	linksToRemove.insert(removingNode->outputLinks.begin(), removingNode->outputLinks.end());

	for (auto link : linksToRemove)
		removeLink(m_caster.castLink(link));
}

template <typename Node_t, typename Link_t>
void Graph<Node_t, Link_t>::removeLink(GraphLinkID link) {
	auto linkIt = m_Links.find(link);
	if (linkIt == m_Links.end())
		return;

	link_ptr removingLink = linkIt->second;
	m_Links.erase(linkIt);

	removeLink(removingLink);
}

template <typename Node_t, typename Link_t>
void Graph<Node_t, Link_t>::removeLink(link_ptr link) {
	link->to->inputLinks.erase(link);
	link->from->outputLinks.erase(link);

	if (!link->isDirected) {
		link->to->outputLinks.erase(link);
		link->from->inputLinks.erase(link);
	}

	link->to.reset();
	link->from.reset();
}

template <typename Node_t, typename Link_t>
bool Graph<Node_t, Link_t>::isNodeExists(GraphNodeID node) {
	return m_Nodes.find(node) != m_Nodes.end();
}

template <typename Node_t, typename Link_t>
bool Graph<Node_t, Link_t>::isLinkExists(GraphLinkID link) {
	return m_Links.find(link) != m_Links.end();
}

template <typename Node_t, typename Link_t>
bool Graph<Node_t, Link_t>::isLinkExists(GraphNodeID from, GraphNodeID to) {
	auto fromIt = m_Nodes.find(from);
	if (fromIt == m_Nodes.end())
		return false;
	for (auto& link : fromIt->second->outputLinks)
		if (link->to->ID == to)
			return true;
	return false;
}

template <typename Node_t, typename Link_t>
std::vector<GraphNodeID> Graph<Node_t, Link_t>::getExternalNodes() {
	std::vector<GraphNodeID> out;
	for (auto& node : m_Nodes)
		if (node.second->inputLinks.empty() && node.second->outputLinks.empty())
			out.push_back(node.second->ID);
	return out;
}

template <typename Node_t, typename Link_t>
void Graph<Node_t, Link_t>::setNodesUnvisited() {
	for (auto& node : m_Nodes) {
		node.second->isVisited = false;
	}
}

template <typename Node_t, typename Link_t>
bool Graph<Node_t, Link_t>::isConnected() {
	setNodesUnvisited();

	std::stack<node_ptr> stackRoute;
	stackRoute.push(m_Nodes.begin()->second);

	while (!stackRoute.empty()) { // using DFS to check connectivity 
		node_ptr node = stackRoute.top();
		stackRoute.pop();
		node->searchingInfo.isVisited = true;

		for (auto& outputLink : node->outputLinks)
			if (!outputLink->to->isVisited)
				stackRoute.push(m_caster.castNode(outputLink->to));
		for (auto& inputLink : node->inputLinks)
			if (!inputLink->from->isVisited)
				stackRoute.push(m_caster.castNode(inputLink->from));
	}

	for (auto& node : m_Nodes) // all nodes should be marked as visited
		if (!node.second->isVisited)
			return false;
	return true;
}

template <typename Node_t, typename Link_t>
void Graph<Node_t, Link_t>::forwardDFS(node_ptr start) {
	std::stack<node_ptr> stackRoute;
	stackRoute.push(start);
	start->isVisited = true;

	while (!stackRoute.empty()) {
		auto curNode = stackRoute.top();
		stackRoute.pop();

		for (auto& outputLink : curNode->outputLinks) {
			auto nextNode = outputLink->to;
			if (!outputLink->isDirected && nextNode == curNode) // processing undirected links
				nextNode = outputLink->from;

			if (nextNode->isVisited)
				continue;

			stackRoute.push(nextNode);
			nextNode->isVisited = true;
		}
	}
}