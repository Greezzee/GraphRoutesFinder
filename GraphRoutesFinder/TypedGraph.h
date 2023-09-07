#pragma once

#include "Graph.h"
#include "NodeType.h"

namespace detail {
	struct GraphTypedNode : public GraphNode
	{
		NodeType type = NodeType::NO_TYPE;
		virtual ~GraphTypedNode() = default;
	};
}

template <typename Node_t = detail::GraphTypedNode,
		  typename Link_t = detail::GraphLink>
class TypedGraph : virtual public Graph<typename Node_t, typename Link_t>
{
public:
	virtual ~TypedGraph() = default;

	GraphLinkID createNode(NodeType type);
	void removeNode(GraphNodeID node);

	// array of nodes that can't be reachable from any node of type
	std::vector<GraphNodeID> getUnreachableNodes(NodeType startType);

	// returns true if at least 1 node of type exists
	bool isNodeOfTypeExists(NodeType type);

protected:
	using node_ptr = std::shared_ptr<Node_t>;
	using link_ptr = std::shared_ptr<Link_t>;

	// Set of nodes of every type
	std::map<NodeType, std::set<node_ptr>> m_typedNodes;
};

template <typename Node_t, typename Link_t>
GraphNodeID TypedGraph<Node_t, Link_t>::createNode(NodeType type) {
	auto newNode = Graph<Node_t, Link_t>::createNewNode();
	newNode.second->type = type;
	m_typedNodes[type].insert(newNode.second);
	return newNode.first;
}

template <typename Node_t, typename Link_t>
void TypedGraph<Node_t, Link_t>::removeNode(GraphNodeID node) {
	auto nodeIt = Graph<Node_t, Link_t>::m_Nodes.find(node);
	if (nodeIt == Graph<Node_t, Link_t>::m_Nodes.end())
		return;

	node_ptr removingNode = nodeIt->second;

	auto typedNodesIt = m_typedNodes.find(removingNode->type);
	if (typedNodesIt != m_typedNodes.end())
		typedNodesIt->second.erase(removingNode);
	Graph<Node_t, Link_t>::removeNode(node);
}

template <typename Node_t, typename Link_t>
std::vector<GraphNodeID> TypedGraph<Node_t, Link_t>::getUnreachableNodes(NodeType startType) {
	Graph<Node_t, Link_t>::setNodesUnvisited();

	auto startNodesTypeIt = m_typedNodes.find(startType);
	if (startNodesTypeIt != m_typedNodes.end()) {
		for (auto& node : startNodesTypeIt->second)
			forwardDFS(node);
	}
	std::vector<GraphNodeID> output;
	for (auto& node : Graph<Node_t, Link_t>::m_Nodes) {
		output.push_back(node.second->ID);
	}

	Graph<Node_t, Link_t>::setNodesUnvisited();
	return output;
}

template <typename Node_t, typename Link_t>
bool TypedGraph<Node_t, Link_t>::isNodeOfTypeExists(NodeType type) {
	auto nodesTypeIt = m_typedNodes.find(type);
	if (nodesTypeIt == m_typedNodes.end())
		return false;
	return !nodesTypeIt->second.empty();
}