#pragma once
#include <functional>
#include <chrono>
#include "TypedGraph.h"
#include "WeightedGraph.h"

namespace detail {

	struct GraphNodeFD : public GraphTypedNode<NodeType> {
		float spaceArea;

		unsigned peopleCapacity, peopleInside = 0;
		float peopleToMoveOut = 0;
		std::vector<std::pair<std::shared_ptr<GraphNodeFD>, float>> peopleToMoveIn;

		float exitCapacity = 0;
		std::shared_ptr<GraphLink> prioritizedDirection;
		std::function<float(float)> foundamentalDiagram;
	};

	template <typename Lenght_t>
	struct GraphLinkFD : public GraphWeightedLink<Lenght_t> {
		Lenght_t passWidth = 0.f;
	};

}

struct PeopleDestributionData {
	unsigned peopleInside = 0;
	unsigned peopleLeft = 0;

	float timeSinceStartSeconds = 0;

	std::map<GraphNodeID, unsigned> peoplePerZone;
};


class PedestrianSimulatorGraphFD final :
	public TypedGraph<NodeType, detail::GraphNodeFD, detail::GraphLinkFD<float>>,
	public WeightedGraph<float, detail::GraphNodeFD, detail::GraphLinkFD<float>>
{
public:
	PedestrianSimulatorGraphFD();

	// node capacity will be calculated automatically
	GraphNodeID createNode(NodeType type, float area);
	GraphNodeID createNode(NodeType type, float area, unsigned zoneCapacity);
	GraphLinkID createLink(GraphNodeID from, GraphNodeID to, float linkWidth);

	void setNodeFoundamentalDiagram(GraphNodeID node, std::function<float(float)> fd);
	void setNodeTypeFoundamentalDiagram(NodeType type, std::function<float(float)> fd);
	void setExitType(NodeType exitType);
	void setNodeExitCapacity(GraphNodeID node, float exitCapacity);
	void setPrioritizedDirection(GraphNodeID node, GraphLinkID prioritizedLink);

	void setPeopleAmountInNode(GraphNodeID, unsigned peopleAmount);
	void fillWithPeopleEvenly(NodeType typeToFill, float fillRate);

	void startSimulation();
	void makeSimulationStep(float deltaTimeSeconds);
	PeopleDestributionData getDestribution();

private:
	float m_timeSinceStartSeconds;
	unsigned m_peopleInsideOnStart;
	NodeType m_exitNodeType;
};