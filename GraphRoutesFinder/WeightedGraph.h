#pragma once
#include "Graph.h"

namespace graphs {

namespace detail {

	template <typename LinkWeight_t>
	struct GraphWeightedLink : public GraphLink
	{
		LinkWeight_t weight = LinkWeight_t();
		virtual ~GraphWeightedLink() = default;

		using link_weight_t = LinkWeight_t;
	};
}

template <typename Node_t, 
		  typename Link_t>
class WeightedGraph : virtual public Graph<typename Node_t, typename Link_t>
{
	using LinkWeight_t = typename Link_t::link_weight_t;

public:
	virtual ~WeightedGraph() = default;

	GraphLinkID createLink(GraphNodeID from, GraphNodeID to, LinkWeight_t weight, bool isDirected = false);
	LinkWeight_t getLinkWeight(GraphLinkID link);
};

template <typename Node_t, typename Link_t>
GraphLinkID WeightedGraph<Node_t, Link_t>::createLink(GraphNodeID from, GraphNodeID to, LinkWeight_t weight, bool isDirected) {
	auto newLink = Graph<Node_t, Link_t>::createNewLink(from, to, isDirected);
	newLink.second->weight = weight;
	return newLink.first;
}

template <typename Node_t, typename Link_t>
typename WeightedGraph<Node_t, Link_t>::LinkWeight_t WeightedGraph<Node_t, Link_t>::getLinkWeight(GraphLinkID link) {
	auto foundLink = this->m_Links.find(link);
	if (foundLink == this->m_Links.end())
		return LinkWeight_t{};

	return this->m_caster.castLink(foundLink.second).weight;
}

} // namespace graphs