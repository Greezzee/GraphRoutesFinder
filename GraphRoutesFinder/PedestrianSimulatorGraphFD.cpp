#include "PedestrianSimulatorGraphFD.h"

using namespace graphs;

namespace graphs::detail {
	const unsigned MAX_PEOPLE_PER_SQ_METER = 5;
	const double TOLERANCE = 0.00001;

	struct Point {
		double x, y;
	};
	
	const std::vector<Point> fdPoints = {
		{0, 0}, {0.5, 0.7}, {1, 1.05}, {1.75, 1.21}, {2.5, 1.4}, {3, 1}, {4, 0.6}, {5, 0.2}, {5.3, 0}
	};

	double StandartFoundamentalDiagram(double density) {
		for (size_t i = 0; i < detail::fdPoints.size() - 1; ++i) {
			if (detail::fdPoints[i].x <= density && density < detail::fdPoints[i + 1].x) {
				double k = (density - detail::fdPoints[i].x) / (detail::fdPoints[i + 1].x - detail::fdPoints[i].x);
				return (1 - k) * detail::fdPoints[i].y + k * detail::fdPoints[i + 1].y;
			}
		}
		return 0;
	}
}

PedestrianSimulatorGraphFD::PedestrianSimulatorGraphFD() :
	m_exitNodeType(NodeType::NO_TYPE), m_peopleInsideOnStart(0), m_timeSinceStartSeconds(0) {}

GraphNodeID PedestrianSimulatorGraphFD::createNode(NodeType type, double area) {
	auto newNode = Graph::createNewNode();
	newNode.second->type = type;
	m_typedNodes[type].insert(newNode.second);
	newNode.second->spaceArea = area;
	newNode.second->peopleCapacity = std::max(static_cast<unsigned>(area * detail::MAX_PEOPLE_PER_SQ_METER), 1u);
	newNode.second->foundamentalDiagram = detail::StandartFoundamentalDiagram;
	return newNode.first;
}

GraphNodeID PedestrianSimulatorGraphFD::createNode(NodeType type, double area, unsigned zoneCapacity) {
	auto newNode = Graph::createNewNode();
	newNode.second->type = type;
	m_typedNodes[type].insert(newNode.second);
	newNode.second->spaceArea = area;
	newNode.second->peopleCapacity = zoneCapacity;
	newNode.second->foundamentalDiagram = detail::StandartFoundamentalDiagram;
	return newNode.first;
}

GraphLinkID PedestrianSimulatorGraphFD::createLink(GraphNodeID from, GraphNodeID to, double linkWidth) {
	auto newLink = Graph::createNewLink(from, to);
	newLink.second->passWidth = linkWidth;

	return newLink.first;
}

void PedestrianSimulatorGraphFD::setNodeFoundamentalDiagram(GraphNodeID node, std::function<double(double)> fd) {
	auto foundNode = m_Nodes.find(node);
	if (foundNode == m_Nodes.end())
		return;
	(*foundNode).second->foundamentalDiagram = fd;
}

void PedestrianSimulatorGraphFD::setNodeTypeFoundamentalDiagram(NodeType type, std::function<double(double)> fd) {
	if (m_typedNodes.find(type) == m_typedNodes.end())
		return;
	for (auto node : m_typedNodes[type])
		node->foundamentalDiagram = fd;
}

void PedestrianSimulatorGraphFD::setExitType(NodeType exitType) {
	m_exitNodeType = exitType;
}

void PedestrianSimulatorGraphFD::setNodeExitCapacity(GraphNodeID node, double exitCapacity) {
	auto foundNode = m_Nodes.find(node);
	if (foundNode == m_Nodes.end())
		return;
	(*foundNode).second->exitCapacity = exitCapacity;
}

void PedestrianSimulatorGraphFD::setPrioritizedDirection(GraphNodeID node, GraphLinkID prioritizedLink) {
	auto foundNode = m_Nodes.find(node);
	auto foundLink = m_Links.find(prioritizedLink);
	if (foundNode == m_Nodes.end() || foundLink == m_Links.end())
		return;
	(*foundNode).second->prioritizedDirection = (*foundLink).second;
}

void PedestrianSimulatorGraphFD::setPeopleAmountInNode(GraphNodeID node, unsigned peopleAmount) {
	auto foundNode = m_Nodes.find(node);
	if (foundNode == m_Nodes.end())
		return;
	(*foundNode).second->peopleInside = peopleAmount;
}

void PedestrianSimulatorGraphFD::fillWithPeopleEvenly(NodeType typeToFill, double fillRate) {
	if (fillRate < 0.0 || fillRate > 1.0)
		return;
	if (m_typedNodes.find(typeToFill) == m_typedNodes.end())
		return;
	for (auto node : m_typedNodes[typeToFill])
		node->peopleInside = static_cast<unsigned>(static_cast<double>(node->peopleCapacity) * fillRate);
}

void PedestrianSimulatorGraphFD::startSimulation() {
	m_timeSinceStartSeconds = 0.0;
	m_peopleInsideOnStart = 0;
	for (auto node : m_Nodes) {
		m_peopleInsideOnStart += node.second->peopleInside;
	}
}

void PedestrianSimulatorGraphFD::makeSimulationStep(double deltaTimeSeconds) {
	for (auto node : m_Nodes) if (node.second->peopleInside > 0) { // every node suggest how many people it wants put into next node
		if (node.second->type != m_exitNodeType) {
			auto nodeTo = m_caster.castNode(node.second->prioritizedDirection->to);
			if (nodeTo->ID == node.second->ID)
				nodeTo = m_caster.castNode(node.second->prioritizedDirection->from);

			double pedestrianDensity;
			if (nodeTo->type == m_exitNodeType)
				pedestrianDensity = static_cast<double>(node.second->peopleInside) / (2 * node.second->spaceArea);
			else
				pedestrianDensity = static_cast<double>(node.second->peopleInside + nodeTo->peopleInside) / (node.second->spaceArea + nodeTo->spaceArea);

			node.second->peopleToMoveOut += node.second->foundamentalDiagram(pedestrianDensity)
											* m_caster.castLink(node.second->prioritizedDirection)->passWidth * deltaTimeSeconds;
			node.second->peopleToMoveOut = std::min(node.second->peopleToMoveOut, static_cast<double>(node.second->peopleInside));
			node.second->peopleToMoveOut = std::max(0., node.second->peopleToMoveOut);
			if (node.second->peopleToMoveOut >= 1. - detail::TOLERANCE) {
				nodeTo->peopleToMoveIn.push_back({ node.second, m_caster.castLink(node.second->prioritizedDirection), static_cast<unsigned>(node.second->peopleToMoveOut) });
				node.second->peopleToMoveOut -= static_cast<unsigned>(node.second->peopleToMoveOut);
			}
		}
		else {
			node.second->peopleToMoveOut += std::min(static_cast<double>(node.second->peopleInside), node.second->exitCapacity * deltaTimeSeconds);
			node.second->peopleToMoveOut = std::min(node.second->peopleToMoveOut, static_cast<double>(node.second->peopleInside));
			node.second->peopleToMoveOut = std::max(0., node.second->peopleToMoveOut);

			if (node.second->peopleToMoveOut >= 1. - detail::TOLERANCE) {
				node.second->peopleInside -= static_cast<unsigned>(node.second->peopleToMoveOut);
				node.second->peopleToMoveOut -= static_cast<unsigned>(node.second->peopleToMoveOut);
			}
		}
	}

	for (auto node : m_Nodes) if (!node.second->peopleToMoveIn.empty()) { // every node calculate how many people it can accept and restricts input to that number
		unsigned maxPeopleToEnter = node.second->peopleCapacity - node.second->peopleInside;
		unsigned peopleWantEnter = 0;
		for (auto& i : node.second->peopleToMoveIn) {
			peopleWantEnter += i.amount;
		}

		if (maxPeopleToEnter >= peopleWantEnter)
			continue;

		float entranceFactor = static_cast<float>(maxPeopleToEnter) / static_cast<float>(peopleWantEnter);
		
		int remainPeople = maxPeopleToEnter;

		for (auto& i : node.second->peopleToMoveIn) { // Проход в переполненную область
			i.amount = static_cast<unsigned>(static_cast<float>(i.amount) * entranceFactor);
			remainPeople -= i.amount;
		}

		while (remainPeople != 0) {
			for (auto& i : node.second->peopleToMoveIn) { // Проход в переполненную область
				i.amount++;
				remainPeople--;
				if (remainPeople == 0)
					break;
			}
		}

	}

	for (auto node : m_Nodes) { // people moving
		for (auto& i : node.second->peopleToMoveIn) {
			node.second->peopleInside += i.amount;
			i.fromNode->peopleInside -= i.amount;
		}
		node.second->peopleToMoveIn.clear();
	}

	m_timeSinceStartSeconds += deltaTimeSeconds;
}

PeopleDestributionData PedestrianSimulatorGraphFD::getDestribution() {
	PeopleDestributionData out;
	
	for (auto node : m_Nodes) {
		out.peopleInside += node.second->peopleInside;
		out.zoneData.insert({ node.first, {node.second->peopleInside, node.second->peopleToMoveOut} });
	}
	out.peopleLeft = m_peopleInsideOnStart - out.peopleInside;
	out.timeSinceStartSeconds = m_timeSinceStartSeconds;
	return out;
}
