#include "PedestrianSimulatorGraphFD.h"

namespace detail {
	const unsigned MAX_PEOPLE_PER_SQ_METER = 4;

	float StandartFoundamentalDiagram(float density) {
		float x = density / 5.f;
		return std::max(0.f, 5.f * (x - x * x));
	}
}

PedestrianSimulatorGraphFD::PedestrianSimulatorGraphFD() :
	m_exitNodeType(NodeType::NO_TYPE), m_peopleInsideOnStart(0), m_timeSinceStartSeconds(0) {}

GraphNodeID PedestrianSimulatorGraphFD::createNode(NodeType type, float area) {
	auto newNode = Graph::createNewNode();
	newNode.second->type = type;
	m_typedNodes[type].insert(newNode.second);
	newNode.second->spaceArea = area;
	newNode.second->peopleCapacity = std::max(static_cast<unsigned>(area * detail::MAX_PEOPLE_PER_SQ_METER), 1u);
	newNode.second->foundamentalDiagram = detail::StandartFoundamentalDiagram;
	return newNode.first;
}

GraphNodeID PedestrianSimulatorGraphFD::createNode(NodeType type, float area, unsigned zoneCapacity) {
	auto newNode = Graph::createNewNode();
	newNode.second->type = type;
	m_typedNodes[type].insert(newNode.second);
	newNode.second->spaceArea = area;
	newNode.second->peopleCapacity = zoneCapacity;
	newNode.second->foundamentalDiagram = detail::StandartFoundamentalDiagram;
	return newNode.first;
}

GraphLinkID PedestrianSimulatorGraphFD::createLink(GraphNodeID from, GraphNodeID to, float linkWidth) {
	auto newLink = Graph::createNewLink(from, to);
	newLink.second->passWidth = linkWidth;

	return newLink.first;
}

void PedestrianSimulatorGraphFD::setNodeFoundamentalDiagram(GraphNodeID node, std::function<float(float)> fd) {
	auto foundNode = m_Nodes.find(node);
	if (foundNode == m_Nodes.end())
		return;
	(*foundNode).second->foundamentalDiagram = fd;
}

void PedestrianSimulatorGraphFD::setNodeTypeFoundamentalDiagram(NodeType type, std::function<float(float)> fd) {
	if (m_typedNodes.find(type) == m_typedNodes.end())
		return;
	for (auto node : m_typedNodes[type])
		node->foundamentalDiagram = fd;
}

void PedestrianSimulatorGraphFD::setExitType(NodeType exitType) {
	m_exitNodeType = exitType;
}

void PedestrianSimulatorGraphFD::setNodeExitCapacity(GraphNodeID node, float exitCapacity) {
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

void PedestrianSimulatorGraphFD::fillWithPeopleEvenly(NodeType typeToFill, float fillRate) {
	if (fillRate < 0.0 || fillRate > 1.0)
		return;
	if (m_typedNodes.find(typeToFill) == m_typedNodes.end())
		return;
	for (auto node : m_typedNodes[typeToFill])
		node->peopleInside = static_cast<unsigned>(static_cast<float>(node->peopleCapacity) * fillRate);
}

void PedestrianSimulatorGraphFD::startSimulation() {
	m_timeSinceStartSeconds = 0.0;
	m_peopleInsideOnStart = 0;
	for (auto node : m_Nodes) {
		m_peopleInsideOnStart += node.second->peopleInside;
	}
}

void PedestrianSimulatorGraphFD::makeSimulationStep(float deltaTimeSeconds) {
	for (auto node : m_Nodes) if (node.second->peopleInside > 0) { // every node suggest how many people it wants put into next node
		if (node.second->type != m_exitNodeType) {
			auto nodeTo = m_caster.castNode(node.second->prioritizedDirection->to);
			if (nodeTo->ID == node.second->ID)
				nodeTo = m_caster.castNode(node.second->prioritizedDirection->from);

			node.second->peopleToMoveOut += node.second->foundamentalDiagram(static_cast<float>(node.second->peopleInside) / node.second->spaceArea)
											* m_caster.castLink(node.second->prioritizedDirection)->passWidth * deltaTimeSeconds;
			node.second->peopleToMoveOut = std::min(node.second->peopleToMoveOut, static_cast<float>(node.second->peopleInside));
			node.second->peopleToMoveOut = std::max(0.f, node.second->peopleToMoveOut);
			nodeTo->peopleToMoveIn.push_back({node.second, node.second->peopleToMoveOut});
		}
		else {
			node.second->peopleToMoveOut += std::min(static_cast<float>(node.second->peopleInside), node.second->exitCapacity * deltaTimeSeconds);
			node.second->peopleToMoveOut = std::min(node.second->peopleToMoveOut, static_cast<float>(node.second->peopleInside));
			node.second->peopleToMoveOut = std::max(0.f, node.second->peopleToMoveOut);
		}
	}

	for (auto node : m_Nodes) if (!node.second->peopleToMoveIn.empty()) { // every node calculate how many people it can accept and restricts input to that number
		unsigned maxPeopleToEnter = node.second->peopleCapacity - node.second->peopleInside;
		unsigned peopleWantEnter = 0;
		for (auto& i : node.second->peopleToMoveIn)
			peopleWantEnter += static_cast<unsigned>(i.second);
		if (maxPeopleToEnter < peopleWantEnter) {
			float enteringFactor = static_cast<float>(maxPeopleToEnter) / static_cast<float>(peopleWantEnter);
			for (auto& i : node.second->peopleToMoveIn) {
				i.second *= enteringFactor;
				i.first->peopleToMoveOut = i.second;
			}
		}
	}

	for (auto node : m_Nodes) { // people moving
		node.second->peopleInside -= static_cast<unsigned>(node.second->peopleToMoveOut);
		node.second->peopleToMoveOut -= static_cast<unsigned>(node.second->peopleToMoveOut);

		for (auto& i : node.second->peopleToMoveIn)
			node.second->peopleInside += static_cast<unsigned>(i.second);
		node.second->peopleToMoveIn.clear();
	}

	m_timeSinceStartSeconds += deltaTimeSeconds;
}

PeopleDestributionData PedestrianSimulatorGraphFD::getDestribution() {
	PeopleDestributionData out;
	
	for (auto node : m_Nodes) {
		out.peopleInside += node.second->peopleInside;
		out.peoplePerZone.insert({ node.first, node.second->peopleInside });
	}
	out.peopleLeft = m_peopleInsideOnStart - out.peopleInside;
	out.timeSinceStartSeconds = m_timeSinceStartSeconds;
	return out;
}
