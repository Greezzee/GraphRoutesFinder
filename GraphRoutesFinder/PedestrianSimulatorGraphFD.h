#pragma once
#include <functional>
#include <chrono>
#include "TypedGraph.h"
#include "WeightedGraph.h"

namespace graphs {

namespace detail {

	struct PeopleToMoveInData;

	struct GraphNodeFD : public GraphTypedNode<NodeType> {
		double spaceArea;

		unsigned peopleCapacity, peopleInside = 0;
		double peopleToMoveOut = 0;
		std::vector<PeopleToMoveInData> peopleToMoveIn;

		double exitCapacity = 0;
		std::shared_ptr<GraphLink> prioritizedDirection;
		std::function<double(double)> foundamentalDiagram;
	};

	template <typename Lenght_t>
	struct GraphLinkFD : public GraphWeightedLink<Lenght_t> {
		Lenght_t passWidth = 0.f;
	};

	struct PeopleToMoveInData {
		std::shared_ptr<GraphNodeFD> fromNode;
		std::shared_ptr<GraphLinkFD<double>> fromLink;
		unsigned amount;
	};

}

struct ZoneData {
	unsigned peopleInside;
	double peopleToMoveOut;
};

struct PeopleDestributionData {
	unsigned peopleInside = 0;
	unsigned peopleLeft = 0;

	double timeSinceStartSeconds = 0;

	std::map<GraphNodeID, ZoneData> zoneData;
};


class PedestrianSimulatorGraphFD final :
	public TypedGraph<NodeType, detail::GraphNodeFD, detail::GraphLinkFD<double>>,
	public WeightedGraph<double, detail::GraphNodeFD, detail::GraphLinkFD<double>>
{
public:
	PedestrianSimulatorGraphFD();

	// node capacity will be calculated automatically
	GraphNodeID createNode(NodeType type, double area);
	GraphNodeID createNode(NodeType type, double area, unsigned zoneCapacity);
	GraphLinkID createLink(GraphNodeID from, GraphNodeID to, double linkWidth);

	void setNodeFoundamentalDiagram(GraphNodeID node, std::function<double(double)> fd);
	void setNodeTypeFoundamentalDiagram(NodeType type, std::function<double(double)> fd);
	void setExitType(NodeType exitType);
	void setNodeExitCapacity(GraphNodeID node, double exitCapacity);
	void setPrioritizedDirection(GraphNodeID node, GraphLinkID prioritizedLink);

	void setPeopleAmountInNode(GraphNodeID, unsigned peopleAmount);
	void fillWithPeopleEvenly(NodeType typeToFill, double fillRate);

	void startSimulation();
	void makeSimulationStep(double deltaTimeSeconds);
	PeopleDestributionData getDestribution();

private:
	double m_timeSinceStartSeconds;
	unsigned m_peopleInsideOnStart;
	NodeType m_exitNodeType;
};

} // namespace graphs