#pragma once
#include "Graph.h"

namespace detail {

	template <typename WeightType>
	struct GraphWeightedLink : public GraphLink
	{
		WeightType weight = WeightType();
		virtual ~GraphWeightedLink() = default;
	};
}

template <typename WeightType = int, 
		  typename Node_t = detail::GraphNode, 
		  typename Link_t = detail::GraphWeightedLink<typename WeightType>>
class WeightedGraph : virtual public Graph<typename Node_t, typename Link_t>
{
public:
	virtual ~WeightedGraph() = default;

	GraphLinkID createLink(GraphNodeID from, GraphNodeID to, WeightType weight, bool isDirected = false);
};

template <typename WeightType, typename Node_t, typename Link_t>
GraphLinkID WeightedGraph<WeightType, Node_t, Link_t>::createLink(GraphNodeID from, GraphNodeID to, WeightType weight, bool isDirected) {
	auto newLink = Graph<Node_t, Link_t>::createNewLink(from, to, isDirected);
	newLink.second->weight = weight;
	return newLink.first;
}