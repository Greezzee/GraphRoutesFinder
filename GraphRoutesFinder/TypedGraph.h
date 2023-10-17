#pragma once
#include "Graph.h"
#include "NodeType.h"

namespace graphs {

namespace detail {
	template <typename NodeType_t>
	struct GraphTypedNode : public GraphNode
	{
		NodeType_t type = NodeType_t{};
		virtual ~GraphTypedNode() = default;

		using node_type_t = NodeType_t;
	};
}

template <typename Node_t,
		  typename Link_t>
class TypedGraph : virtual public Graph<typename Node_t, typename Link_t>
{
	using NodeType_t = typename Node_t::node_type_t;

public:
	virtual ~TypedGraph() = default;

	virtual GraphLinkID createNode(NodeType_t type);
	void removeNode(GraphNodeID node) override;

	// array of nodes that can't be reachable from any node of type
	std::vector<GraphNodeID> getUnreachableNodes(NodeType_t startType);

	// returns true if at least 1 node of type exists
	bool isNodeOfTypeExists(NodeType_t type);

protected:
	using node_ptr = std::shared_ptr<Node_t>;
	using link_ptr = std::shared_ptr<Link_t>;

	// Set of nodes of every type
	std::map<NodeType_t, std::set<node_ptr>> m_typedNodes;
};

template <typename Node_t, typename Link_t>
GraphNodeID TypedGraph<Node_t, Link_t>::createNode(NodeType_t type) {
	auto newNode = this->createNewNode();
	newNode.second->type = type;
	m_typedNodes[type].insert(newNode.second);
	return newNode.first;
}

template <typename Node_t, typename Link_t>
void TypedGraph<Node_t, Link_t>::removeNode(GraphNodeID node) {
	auto nodeIt = this->m_Nodes.find(node);
	if (nodeIt == this->m_Nodes.end())
		return;

	node_ptr removingNode = nodeIt->second;

	auto typedNodesIt = m_typedNodes.find(removingNode->type);
	if (typedNodesIt != m_typedNodes.end())
		typedNodesIt->second.erase(removingNode);
	Graph<Node_t, Link_t>::removeNode(node);
}

template <typename Node_t, typename Link_t>
std::vector<GraphNodeID> TypedGraph<Node_t, Link_t>::getUnreachableNodes(NodeType_t startType) {
	this->setNodesUnvisited();

	auto startNodesTypeIt = m_typedNodes.find(startType);
	if (startNodesTypeIt != m_typedNodes.end()) {
		for (auto& node : startNodesTypeIt->second)
			forwardDFS(node);
	}
	std::vector<GraphNodeID> output;
	for (auto& node : this->m_Nodes) if (node.second->isVisited) {
		output.push_back(node.second->ID);
	}

	this->setNodesUnvisited();
	return output;
}

template <typename Node_t, typename Link_t>
bool TypedGraph<Node_t, Link_t>::isNodeOfTypeExists(NodeType_t type) {
	auto nodesTypeIt = m_typedNodes.find(type);
	if (nodesTypeIt == m_typedNodes.end())
		return false;
	return !nodesTypeIt->second.empty();
}

} // namespace graphs