#pragma once
#include <vector>
#include <algorithm>
#include <random>
#include "PedestrianSimulatorGraphFD.h"
#include "RouteSearchableGraph.h"

#ifdef GRAPHIC_TEST
#include <SFML/Graphics.hpp>
#endif // GRAPHIC_TEST

namespace detail {
	template <typename T>
	struct RandomVar {
		T mean, maxOffset;

		T generate() {
			T offset = static_cast<T>(rand()) / static_cast<T>(RAND_MAX);
			return mean - maxOffset + 2 * offset * maxOffset;
		}
	};

	float randFloat() {
		return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}
}
class GraphTestGenerator final
{
public:
	GraphTestGenerator() :
		m_field(std::vector<std::vector<std::pair<GraphNodeID, float>>>(SIZE_Y, std::vector<std::pair<GraphNodeID, float>>(SIZE_X, { UnsetGraphNodeID, 0.f }))),
		m_nodesArea({ 0, 0 }),
		m_linksWidth({ 0, 0 }),
		m_exitsCapacity({ 0, 0 }),
		m_nodesCount(0),
		m_linksCount(0),
		m_exitsCount(0),
		m_generatingGraph(),
		m_graphToCalcRoutes() {}
	

	void setNodesCount(unsigned nodes) { m_nodesCount = nodes; }
	void setLinksCount(unsigned links) { m_linksCount = links; }
	void setExitsCount(unsigned exits) { m_exitsCount = exits; };
	void setExitsCapacityDestr(float mean, float maxOffset) { m_nodesArea = { mean, maxOffset }; }
	void setNodesAreaDestr(float mean, float maxOffset) { m_linksWidth = { mean, maxOffset }; }
	void setLinksWidthtDestr(float mean, float maxOffset) { m_exitsCapacity = { mean, maxOffset }; }
#ifdef GRAPHIC_TEST
	void initGraphics();
	void drawGraph();
	bool isWindowOpen();
#endif // GRAPHIC_TEST
	PedestrianSimulatorGraphFD generate();
private:
	const size_t SIZE_X = 10, SIZE_Y = 5;

	std::vector<std::vector<std::pair<GraphNodeID, float>>> m_field;
	std::map<std::pair<GraphNodeID, GraphNodeID>, std::pair<GraphLinkID, float>> m_links;
	std::map<std::pair<int, int>, std::pair<GraphNodeID, float>> m_exits;

	detail::RandomVar<float> m_nodesArea, m_linksWidth, m_exitsCapacity;
	unsigned m_nodesCount, m_linksCount, m_exitsCount;

	unsigned m_curNodes = 0, m_curLinks = 0, m_curExits = 0;

	GraphNodeID generateNode(int x, int y, float generation_prob);

	PedestrianSimulatorGraphFD m_generatingGraph;
	RouteSearchableGraph<> m_graphToCalcRoutes;

	std::default_random_engine rng = std::default_random_engine{};

#ifdef GRAPHIC_TEST
	sf::RenderWindow m_window;
	bool m_isSimulating = false;
	float m_timer = 0.;
	sf::Clock m_clock;
#endif // GRAPHIC_TEST
};


PedestrianSimulatorGraphFD GraphTestGenerator::generate() {

	m_curNodes = 0, m_curLinks = 0, m_curExits = 0;
	m_field = std::vector<std::vector<std::pair<GraphNodeID, float>>>(SIZE_Y, std::vector<std::pair<GraphNodeID, float>>(SIZE_X, { UnsetGraphNodeID, 0.f }));
	m_links.clear();
	m_exits.clear();

	m_generatingGraph = PedestrianSimulatorGraphFD();
	m_graphToCalcRoutes = RouteSearchableGraph();

	generateNode(0, 0, .8f);

	if (!m_generatingGraph.isNodeOfTypeExists(NodeType::SOURCE))
		return generate();

	for (size_t y = 0; y < SIZE_Y - 1; ++y)
		for (size_t x = 0; x < SIZE_X; ++x) {
			if (m_field[y][x].first != UnsetGraphNodeID && m_field[y + 1][x].first != UnsetGraphNodeID &&
				(m_links.find({ m_field[y][x].first, m_field[y + 1][x].first }) == m_links.end() &&
				m_links.find({ m_field[y + 1][x].first, m_field[y][x].first }) == m_links.end()) &&
				detail::randFloat() < static_cast<float>(m_linksCount - m_curLinks) / static_cast<float>(m_linksCount)) {

				float width = m_linksWidth.generate();

				float linkWigth = m_linksWidth.generate();
				m_links[{m_field[y][x].first, m_field[y + 1][x].first}] = { m_generatingGraph.createLink(m_field[y][x].first, m_field[y + 1][x].first, linkWigth), linkWigth };
				m_graphToCalcRoutes.createLink(m_field[y][x].first, m_field[y + 1][x].first, 1.f);
				m_curLinks++;
			}
		}

	for (size_t y = 0; y < SIZE_Y; ++y)
		for (size_t x = 0; x < SIZE_X - 1; ++x) {
			if (m_field[y][x].first != UnsetGraphNodeID && m_field[y][x + 1].first != UnsetGraphNodeID &&
				(m_links.find({ m_field[y][x].first, m_field[y][x + 1].first }) == m_links.end() &&
				m_links.find({ m_field[y][x + 1].first, m_field[y][x].first }) == m_links.end()) &&
				detail::randFloat() < static_cast<float>(m_linksCount - m_curLinks) / static_cast<float>(m_linksCount)) {

				float width = m_linksWidth.generate();

				float linkWigth = m_linksWidth.generate();
				m_links[{m_field[y][x].first, m_field[y][x + 1].first}] = { m_generatingGraph.createLink(m_field[y][x].first, m_field[y][x + 1].first, linkWigth), linkWigth };
				m_graphToCalcRoutes.createLink(m_field[y][x].first, m_field[y][x + 1].first, 1.f);
				m_curLinks++;
			}
		}

	auto routes = m_graphToCalcRoutes.findShortestRoutes(NodeType::SOURCE, NodeType::STANDART);

	for (auto route : routes) {
		m_generatingGraph.setPrioritizedDirection(route.node, route.routesData[0].linksRoute[0]);
	}

	m_generatingGraph.setExitType(NodeType::SOURCE);

	return m_generatingGraph;
}

GraphNodeID GraphTestGenerator::generateNode(int x, int y, float generation_prob) {

	float exitProb = static_cast<float>(m_exitsCount - m_curExits) / static_cast<float>(m_nodesCount);

	NodeType typeToCreate = NodeType::STANDART;

	if (detail::randFloat() < exitProb) {
		typeToCreate = NodeType::SOURCE;
		m_curExits++;
	}
	else {
		typeToCreate = NodeType::STANDART;
	}

	float area = m_nodesArea.generate();
	m_field[y][x] = { m_generatingGraph.createNode(typeToCreate, area), area };

	if (typeToCreate == NodeType::SOURCE) {
		float capacity = m_exitsCapacity.generate();
		m_generatingGraph.setNodeExitCapacity(m_field[y][x].first, capacity);
		m_exits[{x, y}] = { m_field[y][x].first, capacity };
	}

	m_graphToCalcRoutes.createNode(typeToCreate);
	m_curNodes++;

	if (m_curNodes == m_nodesCount)
		return m_field[y][x].first;

	std::vector<std::pair<int, int>> nearbyNodes;
	if (x > 0 && m_field[y][x - 1].first == UnsetGraphNodeID)
		nearbyNodes.push_back({ x - 1, y });
	if (y > 0 && m_field[y - 1][x].first == UnsetGraphNodeID)
		nearbyNodes.push_back({ x, y - 1 });
	if (x < SIZE_X - 1 && m_field[y][x + 1].first == UnsetGraphNodeID)
		nearbyNodes.push_back({ x + 1, y });
	if (y < SIZE_Y - 1 && m_field[y + 1][x].first == UnsetGraphNodeID)
		nearbyNodes.push_back({ x, y + 1 });

	std::shuffle(nearbyNodes.begin(), nearbyNodes.end(), rng);

	for (int i = 0; i < nearbyNodes.size(); i++) if (m_curNodes < m_nodesCount && detail::randFloat() < generation_prob) {
		GraphNodeID nextNode = generateNode(nearbyNodes[i].first, nearbyNodes[i].second, generation_prob);
		float linkWigth = m_linksWidth.generate();
		m_links[{m_field[y][x].first, nextNode}] = { m_generatingGraph.createLink(m_field[y][x].first, nextNode, linkWigth), linkWigth };
		m_graphToCalcRoutes.createLink(m_field[y][x].first, nextNode, 1.f);
		m_curLinks++;
	}
	return m_field[y][x].first;
}

#ifdef GRAPHIC_TEST
void GraphTestGenerator::initGraphics() {
	m_window.create(sf::VideoMode(1900, 950), "PedestrianSimulationDemo");
	m_window.setKeyRepeatEnabled(false);
	generate();
	m_generatingGraph.startSimulation();
	m_generatingGraph.fillWithPeopleEvenly(NodeType::STANDART, 0.5);
	m_clock.restart();
}

void GraphTestGenerator::drawGraph() {
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			m_window.close();
		if (event.type == sf::Event::KeyPressed) {
			switch (event.type)
			{
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::R) {
					generate();
					m_generatingGraph.startSimulation();
					m_generatingGraph.fillWithPeopleEvenly(NodeType::STANDART, 0.5);
					m_isSimulating = false;
				}
				if (event.key.code == sf::Keyboard::Space) {
					m_isSimulating = !m_isSimulating;
				}
				break;
			default:
				break;
			}
		}
	}

	float dt = m_clock.restart().asSeconds();

	if (m_isSimulating) {
		m_timer += dt;
		const float simulationStep = 0.2;
		if (m_timer > simulationStep) {
			m_timer -= simulationStep;
			m_generatingGraph.makeSimulationStep(simulationStep);
		}
	}

	m_window.clear(sf::Color(50, 50, 50, 255));

	auto data = m_generatingGraph.getDestribution();

	for (int x = 0; x < SIZE_X; ++x)
		for (int y = 0; y < SIZE_Y; ++y) if (m_field[y][x].first != UnsetGraphNodeID) {
			
			float baseSize = 190;

			sf::RectangleShape shape(sf::Vector2f(baseSize, baseSize));
			shape.setOrigin(baseSize / 2., baseSize / 2.);

			shape.setFillColor(sf::Color(100, 100, 100));
			if (x + 1 < SIZE_X && m_field[y][x + 1].first != UnsetGraphNodeID &&
				(m_links.find({ m_field[y][x].first, m_field[y][x + 1].first }) != m_links.end() ||
				 m_links.find({ m_field[y][x + 1].first, m_field[y][x].first }) != m_links.end())) {

				auto link = m_links.find({ m_field[y][x].first, m_field[y][x + 1].first });
				if (link == m_links.end())
					link = m_links.find({ m_field[y][x + 1].first, m_field[y][x].first });

				shape.setScale(0.9, 0.4 * link->second.second / (m_linksWidth.mean + m_linksWidth.maxOffset));
				shape.setPosition(baseSize / 2. + baseSize * x + baseSize / 2., baseSize / 2. + baseSize * y);
				m_window.draw(shape);
			}

			if (y + 1 < SIZE_Y && m_field[y + 1][x].first != UnsetGraphNodeID &&
				(m_links.find({ m_field[y + 1][x].first, m_field[y][x].first }) != m_links.end() ||
					m_links.find({ m_field[y][x].first, m_field[y + 1][x].first }) != m_links.end())) {

				auto link = m_links.find({ m_field[y + 1][x].first, m_field[y][x].first });
				if (link == m_links.end())
					link = m_links.find({ m_field[y][x].first, m_field[y + 1][x].first });

				shape.setScale(0.4 * link->second.second / (m_linksWidth.mean + m_linksWidth.maxOffset), 0.9);

				shape.setPosition(baseSize / 2. + baseSize * x, baseSize / 2. + baseSize * y + baseSize / 2.);
				m_window.draw(shape);
			}
		}

	for (int x = 0; x < SIZE_X; ++x)
		for (int y = 0; y < SIZE_Y; ++y) if (m_field[y][x].first != UnsetGraphNodeID) {

			float baseSize = 190;

			sf::RectangleShape shape(sf::Vector2f(baseSize, baseSize));
			shape.setOrigin(baseSize / 2., baseSize / 2.);

			shape.setScale(0.9 * m_field[y][x].second / (m_nodesArea.mean + m_nodesArea.maxOffset), 0.9 * m_field[y][x].second / (m_nodesArea.mean + m_nodesArea.maxOffset));
			shape.setFillColor(sf::Color::Green);

			shape.setPosition(baseSize / 2. + baseSize * x, baseSize / 2. + baseSize * y);
			m_window.draw(shape);

			if (m_exits.find({ x, y }) != m_exits.end()) {
				shape.setFillColor(sf::Color::Transparent);
				shape.setOutlineThickness(10);
				shape.setOutlineColor(sf::Color::Blue);
				m_window.draw(shape);
			}
			shape.setOutlineThickness(0);
		}

	for (int x = 0; x < SIZE_X; ++x)
		for (int y = 0; y < SIZE_Y; ++y) if (m_field[y][x].first != UnsetGraphNodeID) {

			float baseSize = 190;

			sf::Text textInfo;
			sf::Font font;
			font.loadFromFile("../Roboto-Black.ttf");
			textInfo.setFont(font);
			textInfo.setCharacterSize(baseSize / 10.);
			textInfo.setFillColor(sf::Color::Black);

			if (x + 1 < SIZE_X && m_field[y][x + 1].first != UnsetGraphNodeID &&
				(m_links.find({ m_field[y][x].first, m_field[y][x + 1].first }) != m_links.end() ||
					m_links.find({ m_field[y][x + 1].first, m_field[y][x].first }) != m_links.end())) {

				auto link = m_links.find({ m_field[y][x].first, m_field[y][x + 1].first });
				if (link == m_links.end())
					link = m_links.find({ m_field[y][x + 1].first, m_field[y][x].first });

				textInfo.setString(std::to_string(link->second.second).substr(0, 4));
				textInfo.setPosition(baseSize / 2. + baseSize * x + baseSize / 2. - baseSize / 10., baseSize / 2. + baseSize * y - baseSize / 20.);
				m_window.draw(textInfo);
			}

			if (y + 1 < SIZE_Y && m_field[y + 1][x].first != UnsetGraphNodeID &&
				(m_links.find({ m_field[y + 1][x].first, m_field[y][x].first }) != m_links.end() ||
					m_links.find({ m_field[y][x].first, m_field[y + 1][x].first }) != m_links.end())) {

				auto link = m_links.find({ m_field[y + 1][x].first, m_field[y][x].first });
				if (link == m_links.end())
					link = m_links.find({ m_field[y][x].first, m_field[y + 1][x].first });

				textInfo.setString(std::to_string(link->second.second).substr(0, 4));
				textInfo.setPosition(baseSize * x + baseSize / 2. - baseSize / 10., baseSize / 2. + baseSize / 2. + baseSize * y - baseSize / 20.);
				m_window.draw(textInfo);
			}

			textInfo.setString(std::to_string(m_field[y][x].second).substr(0, 4));
			textInfo.setPosition(baseSize / 2. + baseSize * x - baseSize / 10., baseSize / 2. + baseSize * y - baseSize / 10.);
			m_window.draw(textInfo);

			textInfo.setString(std::to_string(data.peoplePerZone[m_field[y][x].first]).substr(0, 4));
			textInfo.setPosition(baseSize / 2. + baseSize * x - baseSize / 10., baseSize / 2. + baseSize * y);
			m_window.draw(textInfo);
		}
	m_window.display();
}

bool GraphTestGenerator::isWindowOpen() {
	return m_window.isOpen();
}
#endif // GRAPHIC_TEST
