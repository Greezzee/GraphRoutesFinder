#pragma once
#include "Graph.h"

namespace graphs {

namespace detail {

	template <typename LinkWeight_t>
	struct GraphWeightedLink : public GraphLink
	{
		LinkWeight_t weight = LinkWeight_t();
		virtual ~GraphWeightedLink() = default;
	};
}

template <typename LinkWeight_t,
		  typename Node_t, 
		  typename Link_t>
class WeightedGraph : virtual public Graph<typename Node_t, typename Link_t>
{
public:
	virtual ~WeightedGraph() = default;

	GraphLinkID createLink(GraphNodeID from, GraphNodeID to, LinkWeight_t weight, bool isDirected = false);
};

template <typename LinkWeight_t, typename Node_t, typename Link_t>
GraphLinkID WeightedGraph<LinkWeight_t, Node_t, Link_t>::createLink(GraphNodeID from, GraphNodeID to, LinkWeight_t weight, bool isDirected) {
	auto newLink = Graph<Node_t, Link_t>::createNewLink(from, to, isDirected);
	newLink.second->weight = weight;
	return newLink.first;
}

} // namespace graphs