#pragma once
#include <vector>
#include <algorithm>
#include <random>
#include "../GraphRoutesFinder/PedestrianSimulatorGraphFD.h"
#include "../GraphRoutesFinder/RouteSearchableGraph.h"

#include <SFML/Graphics.hpp>

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
		m_field(std::vector<std::vector<std::pair<graphs::GraphNodeID, float>>>(SIZE_Y, std::vector<std::pair<graphs::GraphNodeID, float>>(SIZE_X, { graphs::UnsetGraphNodeID, 0.f }))),
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

	void initGraphics();
	void drawGraph();
	bool isWindowOpen();
	graphs::PedestrianSimulatorGraphFD generate();
private:

	const size_t SIZE_X = 10, SIZE_Y = 5;

	std::vector<std::vector<std::pair<graphs::GraphNodeID, float>>> m_field;
	std::map<std::pair<graphs::GraphNodeID, graphs::GraphNodeID>, std::pair<graphs::GraphLinkID, float>> m_links;
	std::map<std::pair<int, int>, std::pair<graphs::GraphNodeID, float>> m_exits;

	detail::RandomVar<float> m_nodesArea, m_linksWidth, m_exitsCapacity;
	unsigned m_nodesCount, m_linksCount, m_exitsCount;

	unsigned m_curNodes = 0, m_curLinks = 0, m_curExits = 0;

	graphs::GraphNodeID generateNode(int x, int y, float generation_prob);

	graphs::PedestrianSimulatorGraphFD m_generatingGraph;
	graphs::RouteSearchableGraph<> m_graphToCalcRoutes;

	std::default_random_engine rng = std::default_random_engine{};

	sf::RenderWindow m_window;
	bool m_isSimulating = false;
	float m_timer = 0., m_simulationTime = 0.;
	sf::Clock m_clock;
};


graphs::PedestrianSimulatorGraphFD GraphTestGenerator::generate() {

	m_curNodes = 0, m_curLinks = 0, m_curExits = 0;
	m_field = std::vector<std::vector<std::pair<graphs::GraphNodeID, float>>>(SIZE_Y, std::vector<std::pair<graphs::GraphNodeID, float>>(SIZE_X, { graphs::UnsetGraphNodeID, 0.f }));
	m_links.clear();
	m_exits.clear();

	m_generatingGraph = graphs::PedestrianSimulatorGraphFD();
	m_graphToCalcRoutes = graphs::RouteSearchableGraph();
	generateNode(0, 0, .8f);

	if (!m_generatingGraph.isNodeOfTypeExists(graphs::NodeType::SOURCE))
		return generate();

	for (size_t y = 0; y < SIZE_Y - 1; ++y)
		for (size_t x = 0; x < SIZE_X; ++x) {
			if (m_field[y][x].first != graphs::UnsetGraphNodeID && m_field[y + 1][x].first != graphs::UnsetGraphNodeID &&
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
			if (m_field[y][x].first != graphs::UnsetGraphNodeID && m_field[y][x + 1].first != graphs::UnsetGraphNodeID &&
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

	auto routes = m_graphToCalcRoutes.findShortestRoutes(graphs::NodeType::SOURCE, graphs::NodeType::STANDART);

	for (auto route : routes) {
		m_generatingGraph.setPrioritizedDirection(route.node, route.routesData[0].linksRoute.back());
	}

	m_generatingGraph.setExitType(graphs::NodeType::SOURCE);

	return m_generatingGraph;
}

graphs::GraphNodeID GraphTestGenerator::generateNode(int x, int y, float generation_prob) {

	float exitProb = static_cast<float>(m_exitsCount - m_curExits) / static_cast<float>(m_nodesCount);

	graphs::NodeType typeToCreate = graphs::NodeType::STANDART;

	if (detail::randFloat() < exitProb) {
		typeToCreate = graphs::NodeType::SOURCE;
		m_curExits++;
	}
	else {
		typeToCreate = graphs::NodeType::STANDART;
	}

	float area = m_nodesArea.generate();
	m_field[y][x] = { m_generatingGraph.createNode(typeToCreate, area), area };

	if (typeToCreate == graphs::NodeType::SOURCE) {
		float capacity = m_exitsCapacity.generate();
		m_generatingGraph.setNodeExitCapacity(m_field[y][x].first, capacity);
		m_exits[{x, y}] = { m_field[y][x].first, capacity };
	}

	m_graphToCalcRoutes.createNode(typeToCreate);
	m_curNodes++;

	if (m_curNodes == m_nodesCount)
		return m_field[y][x].first;

	std::vector<std::pair<int, int>> nearbyNodes;
	if (x > 0 && m_field[y][x - 1].first == graphs::UnsetGraphNodeID)
		nearbyNodes.push_back({ x - 1, y });
	if (y > 0 && m_field[y - 1][x].first == graphs::UnsetGraphNodeID)
		nearbyNodes.push_back({ x, y - 1 });
	if (x < SIZE_X - 1 && m_field[y][x + 1].first == graphs::UnsetGraphNodeID)
		nearbyNodes.push_back({ x + 1, y });
	if (y < SIZE_Y - 1 && m_field[y + 1][x].first == graphs::UnsetGraphNodeID)
		nearbyNodes.push_back({ x, y + 1 });

	std::shuffle(nearbyNodes.begin(), nearbyNodes.end(), rng);

	for (int i = 0; i < nearbyNodes.size(); i++) if (m_curNodes < m_nodesCount && detail::randFloat() < generation_prob) {
		graphs::GraphNodeID nextNode = generateNode(nearbyNodes[i].first, nearbyNodes[i].second, generation_prob);
		float linkWigth = m_linksWidth.generate();
		m_links[{m_field[y][x].first, nextNode}] = { m_generatingGraph.createLink(m_field[y][x].first, nextNode, linkWigth), linkWigth };
		m_graphToCalcRoutes.createLink(m_field[y][x].first, nextNode, 1.f);
		m_curLinks++;
	}
	return m_field[y][x].first;
}

void GraphTestGenerator::initGraphics() {
	m_window.create(sf::VideoMode(1900, 950), "PedestrianSimulationDemo");
	m_window.setKeyRepeatEnabled(false);
	generate();
	m_generatingGraph.startSimulation();
	m_generatingGraph.fillWithPeopleEvenly(graphs::NodeType::STANDART, 0.5);
	m_clock.restart();
}

namespace {
	float fillRatio = 0.5;
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
					m_generatingGraph.fillWithPeopleEvenly(graphs::NodeType::STANDART, fillRatio);
					m_isSimulating = false;
					m_simulationTime = 0.;
				}
				if (event.key.code == sf::Keyboard::C) {
					m_generatingGraph.startSimulation();
					m_generatingGraph.fillWithPeopleEvenly(graphs::NodeType::STANDART, fillRatio);
					m_generatingGraph.fillWithPeopleEvenly(graphs::NodeType::SOURCE, 0.);
					m_isSimulating = false;
					m_simulationTime = 0.;
				}
				if (event.key.code == sf::Keyboard::Up && !m_isSimulating) {
					fillRatio = std::min(fillRatio + 0.1f, 1.f);
					m_generatingGraph.fillWithPeopleEvenly(graphs::NodeType::STANDART, fillRatio);
				}
				if (event.key.code == sf::Keyboard::Down && !m_isSimulating) {
					fillRatio = std::max(fillRatio - 0.1f, 0.f);
					m_generatingGraph.fillWithPeopleEvenly(graphs::NodeType::STANDART, fillRatio);
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
		m_simulationTime += dt;
		const float simulationStep = 0.2f;
		if (m_timer > simulationStep) {
			m_timer -= simulationStep;
			m_generatingGraph.makeSimulationStep(simulationStep);
		}
	}

	m_window.clear(sf::Color(50, 50, 50, 255));

	auto data = m_generatingGraph.getDestribution();

	for (int x = 0; x < SIZE_X; ++x)
		for (int y = 0; y < SIZE_Y; ++y) if (m_field[y][x].first != graphs::UnsetGraphNodeID) {
			
			float baseSize = 190;

			sf::RectangleShape shape(sf::Vector2f(baseSize, baseSize));
			shape.setOrigin(baseSize / 2.f, baseSize / 2.f);

			shape.setFillColor(sf::Color(100, 100, 100));
			if (x + 1 < SIZE_X && m_field[y][x + 1].first != graphs::UnsetGraphNodeID &&
				(m_links.find({ m_field[y][x].first, m_field[y][x + 1].first }) != m_links.end() ||
				 m_links.find({ m_field[y][x + 1].first, m_field[y][x].first }) != m_links.end())) {

				auto link = m_links.find({ m_field[y][x].first, m_field[y][x + 1].first });
				if (link == m_links.end())
					link = m_links.find({ m_field[y][x + 1].first, m_field[y][x].first });

				shape.setScale(0.9f, 0.4f * link->second.second / (m_linksWidth.mean + m_linksWidth.maxOffset));
				shape.setPosition(baseSize / 2.f + baseSize * x + baseSize / 2.f, baseSize / 2.f + baseSize * y);
				m_window.draw(shape);
			}

			if (y + 1 < SIZE_Y && m_field[y + 1][x].first != graphs::UnsetGraphNodeID &&
				(m_links.find({ m_field[y + 1][x].first, m_field[y][x].first }) != m_links.end() ||
					m_links.find({ m_field[y][x].first, m_field[y + 1][x].first }) != m_links.end())) {

				auto link = m_links.find({ m_field[y + 1][x].first, m_field[y][x].first });
				if (link == m_links.end())
					link = m_links.find({ m_field[y][x].first, m_field[y + 1][x].first });

				shape.setScale(0.4f * link->second.second / (m_linksWidth.mean + m_linksWidth.maxOffset), 0.9f);

				shape.setPosition(baseSize / 2.f + baseSize * x, baseSize / 2.f + baseSize * y + baseSize / 2.f);
				m_window.draw(shape);
			}
		}

	for (int x = 0; x < SIZE_X; ++x)
		for (int y = 0; y < SIZE_Y; ++y) if (m_field[y][x].first != graphs::UnsetGraphNodeID) {

			float baseSize = 190;

			sf::RectangleShape shape(sf::Vector2f(baseSize, baseSize));
			shape.setOrigin(baseSize / 2.f, baseSize / 2.f);

			shape.setScale(0.9f * m_field[y][x].second / (m_nodesArea.mean + m_nodesArea.maxOffset), 0.9f * m_field[y][x].second / (m_nodesArea.mean + m_nodesArea.maxOffset));

			float red = static_cast<float>(data.zoneData[m_field[y][x].first].peopleInside) / (m_field[y][x].second * 4.f) * 255.f;

			shape.setFillColor(sf::Color(255, 255 - red, 255 - red));

			shape.setPosition(baseSize / 2.f + baseSize * x, baseSize / 2.f + baseSize * y);
			m_window.draw(shape);

			if (m_exits.find({ x, y }) != m_exits.end()) {
				shape.setFillColor(sf::Color::Transparent);
				shape.setOutlineThickness(10);
				shape.setOutlineColor(sf::Color::Blue);
				m_window.draw(shape);
			}
			shape.setOutlineThickness(0);
		}

	float baseSize = 190;
	sf::Text textInfo;
	sf::Font font;
	font.loadFromFile("../Roboto-Black.ttf");
	textInfo.setFont(font);
	textInfo.setCharacterSize(baseSize / 10.f);
	textInfo.setFillColor(sf::Color::Black);

	for (int x = 0; x < SIZE_X; ++x)
		for (int y = 0; y < SIZE_Y; ++y) if (m_field[y][x].first != graphs::UnsetGraphNodeID) {

			if (x + 1 < SIZE_X && m_field[y][x + 1].first != graphs::UnsetGraphNodeID &&
				(m_links.find({ m_field[y][x].first, m_field[y][x + 1].first }) != m_links.end() ||
					m_links.find({ m_field[y][x + 1].first, m_field[y][x].first }) != m_links.end())) {

				auto link = m_links.find({ m_field[y][x].first, m_field[y][x + 1].first });
				if (link == m_links.end())
					link = m_links.find({ m_field[y][x + 1].first, m_field[y][x].first });

				textInfo.setString(std::to_string(link->second.second).substr(0, 4));
				textInfo.setPosition(baseSize / 2.f + baseSize * x + baseSize / 2.f - baseSize / 10.f, baseSize / 2.f + baseSize * y - baseSize / 20.f);
				m_window.draw(textInfo);
			}

			if (y + 1 < SIZE_Y && m_field[y + 1][x].first != graphs::UnsetGraphNodeID &&
				(m_links.find({ m_field[y + 1][x].first, m_field[y][x].first }) != m_links.end() ||
					m_links.find({ m_field[y][x].first, m_field[y + 1][x].first }) != m_links.end())) {

				auto link = m_links.find({ m_field[y + 1][x].first, m_field[y][x].first });
				if (link == m_links.end())
					link = m_links.find({ m_field[y][x].first, m_field[y + 1][x].first });

				textInfo.setString(std::to_string(link->second.second).substr(0, 4));
				textInfo.setPosition(baseSize * x + baseSize / 2.f - baseSize / 10.f, baseSize / 2.f + baseSize / 2.f + baseSize * y - baseSize / 20.f);
				m_window.draw(textInfo);
			}

			textInfo.setString(std::to_string(m_field[y][x].second).substr(0, 4));
			textInfo.setPosition(baseSize / 2.f + baseSize * x - baseSize / 10.f, baseSize / 2.f + baseSize * y - baseSize / 10.f);
			m_window.draw(textInfo);

			textInfo.setString(std::to_string(data.zoneData[m_field[y][x].first].peopleInside).substr(0, 4));
			textInfo.setPosition(baseSize / 2.f + baseSize * x - baseSize / 10.f, baseSize / 2.f + baseSize * y);
			m_window.draw(textInfo);

			textInfo.setString(std::to_string(data.zoneData[m_field[y][x].first].peopleToMoveOut).substr(0, 4));
			textInfo.setPosition(baseSize / 2.f + baseSize * x - baseSize / 10.f, baseSize / 2.f + baseSize * y + baseSize / 10.f);
			m_window.draw(textInfo);
		}

	textInfo.setCharacterSize(baseSize / 5.f);
	textInfo.setFillColor(sf::Color::White);
	textInfo.setString(std::to_string(m_simulationTime).substr(0, 4));
	textInfo.setPosition(baseSize * 9.5f, baseSize / 10.f);
	m_window.draw(textInfo);

	m_window.display();
}

bool GraphTestGenerator::isWindowOpen() {
	return m_window.isOpen();
}
