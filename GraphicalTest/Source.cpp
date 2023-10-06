#include <iostream>
#include "GraphTestGenerator.h"
#include <SFML/Graphics.hpp>
/*
int main()
{
	GraphTestGenerator generator;

	generator.setNodesCount(30);
	generator.setLinksCount(50);
	generator.setExitsCount(3);
	generator.setExitsCapacityDestr(5, 4);
	generator.setLinksWidthtDestr(5, 4);
	generator.setNodesAreaDestr(5, 3);

	generator.initGraphics();

	while (generator.isWindowOpen())
		generator.drawGraph();

	return 0;
}
*/

using namespace graphs;

struct GraphZone {
	int x_idx, y_idx;

	float x_pos, y_pos;
	float x_len, y_len;

	GraphNodeID node_id = UnsetGraphNodeID;
};

int main() {
	PedestrianSimulatorGraphFD graph;
	RouteSearchableGraph routeGraph;

	std::vector<GraphZone> zones;

	std::vector<float> x_poses = { 1.5, 2, 3, 2, 3, 2, 2.5, 2, 1.5, 2, 1.5 };
	std::vector<float> y_poses_1 = { 1, 3, 1.5, 3, 1.5, 3, 1.5, 2.5, 1 };
	std::vector<float> y_poses_2 = { 1, 2, 1.5, 2, 1.5, 2, 1.5, 2, 1.5, 2, 1 };
	std::vector<float> y_poses_3 = { 1, 2, 3.5, 3.5, 1.5, 3.5, 2, 1 };

	float x_pos = 0, y_pos = 0;

	for (int x = 0; x < 6; x++) {
		for (int y = 0; y < 9; y++) {
			GraphZone newZone;
			newZone.x_idx = x;
			newZone.y_idx = y;
			newZone.x_pos = x_pos;
			newZone.y_pos = y_pos;
			newZone.x_len = x_poses[x];
			newZone.y_len = y_poses_1[y];

			if (x % 2 == 0 || y % 2 == 0) {
				zones.push_back(newZone);
			}

			y_pos += y_poses_1[y];
		}
		x_pos += x_poses[x];
		y_pos = 0;
	}

	for (int x = 6; x < 7; x++) {
		for (int y = 0; y < 8; y++) {
			GraphZone newZone;
			newZone.x_idx = x;
			newZone.y_idx = y;
			newZone.x_pos = x_pos;
			newZone.y_pos = y_pos;
			newZone.x_len = x_poses[x];
			newZone.y_len = y_poses_3[y];

			zones.push_back(newZone);

			y_pos += y_poses_3[y];
		}
		x_pos += x_poses[x];
		y_pos = 0;
	}

	for (int x = 7; x < 11; x++) {
		for (int y = 0; y < 11; y++) {
			GraphZone newZone;
			newZone.x_idx = x;
			newZone.y_idx = y;
			newZone.x_pos = x_pos;
			newZone.y_pos = y_pos;
			newZone.x_len = x_poses[x];
			newZone.y_len = y_poses_2[y];

			if ((x % 2 == 0 || y % 2 == 0) && !(x == 8 && y == 5)) {
				zones.push_back(newZone);
			}

			y_pos += y_poses_2[y];
		}
		x_pos += x_poses[x];
		y_pos = 0;
	}


	float x_full_len = 0;
	float y_full_len = 0;

	for (int i = 0; i < x_poses.size(); ++i)
		x_full_len += x_poses[i];
	for (int i = 0; i < y_poses_1.size(); ++i)
		y_full_len += y_poses_1[i];


	for (int i = 0; i < zones.size(); ++i) {
		zones[i].node_id = graph.createNode(NodeType::STANDART, zones[i].x_len * zones[i].y_len);
		routeGraph.createNode(NodeType::STANDART);
	}

	auto exitA = graph.createNode(NodeType::SOURCE, 9999.);
	routeGraph.createNode(NodeType::SOURCE);
	graph.setNodeExitCapacity(exitA, 30.5);
	auto exitB = graph.createNode(NodeType::SOURCE, 9999.);
	routeGraph.createNode(NodeType::SOURCE);
	graph.setNodeExitCapacity(exitB, 20.5);

	graph.setExitType(NodeType::SOURCE);

	for (int i = 0; i < zones.size(); ++i) {
		for (int j = 0; j < zones.size(); ++j) {

			if (zones[i].x_idx == zones[j].x_idx && zones[i].x_idx % 2 == 0 && zones[i].y_idx == zones[j].y_idx + 1) {

				if (!(zones[i].x_idx == 8 && zones[i].y_idx == 5)) {
					graph.createLink(zones[i].node_id, zones[j].node_id, zones[i].x_len);
					routeGraph.createLink(zones[i].node_id, zones[j].node_id, (zones[i].y_len + zones[j].y_len) / 2.);
				}

			}

			if (zones[i].y_idx == zones[j].y_idx && zones[i].y_idx % 2 == 0 && zones[i].x_idx == zones[j].x_idx + 1) {
				if (zones[i].x_idx != 6 && zones[j].x_idx != 6) {
					graph.createLink(zones[i].node_id, zones[j].node_id, zones[i].y_len);
					routeGraph.createLink(zones[i].node_id, zones[j].node_id, (zones[i].x_len + zones[j].x_len) / 2.);
				}
			}

			if (zones[i].x_idx == 6 && zones[j].x_idx == 5) {
				if (zones[j].y_idx == 0 && zones[i].y_idx == 0 ||
					zones[j].y_idx == 2 && zones[i].y_idx == 2 ||
					zones[j].y_idx == 4 && zones[i].y_idx == 3 ||
					zones[j].y_idx == 6 && zones[i].y_idx == 5 ||
					zones[j].y_idx == 8 && zones[i].y_idx == 7) {

					graph.createLink(zones[i].node_id, zones[j].node_id, zones[j].y_len);
					routeGraph.createLink(zones[i].node_id, zones[j].node_id, (zones[i].x_len + zones[j].x_len) / 2.);
				}
			}

			if (zones[i].x_idx == 6 && zones[j].x_idx == 7) {
				if (zones[j].y_idx == 0 && zones[i].y_idx == 0 ||
					zones[j].y_idx == 2 && zones[i].y_idx == 2 ||
					zones[j].y_idx == 4 && zones[i].y_idx == 3 ||
					zones[j].y_idx == 6 && zones[i].y_idx == 4 ||
					zones[j].y_idx == 8 && zones[i].y_idx == 5 ||
					zones[j].y_idx == 10 && zones[i].y_idx == 7) {

					graph.createLink(zones[i].node_id, zones[j].node_id, zones[j].y_len);
					routeGraph.createLink(zones[i].node_id, zones[j].node_id, (zones[i].x_len + zones[j].x_len) / 2.);
				}
			}
		}

		if (zones[i].x_idx == 4 && zones[i].y_idx == 8) {
			graph.createLink(zones[i].node_id, exitB, 1.5);
			routeGraph.createLink(zones[i].node_id, exitB, (zones[i].x_len) / 2.);
		}
		if (zones[i].x_idx == 10 && zones[i].y_idx == 7) {
			graph.createLink(zones[i].node_id, exitA, 2);
			routeGraph.createLink(zones[i].node_id, exitA, (zones[i].x_len) / 2.);
		}
	}

	auto routes = routeGraph.findShortestRoutes(NodeType::SOURCE, NodeType::STANDART);

	for (size_t i = 0; i < routes.size(); i++)
		for (size_t j = 0; j < zones.size(); j++) if (routes[i].node == zones[j].node_id) {
			graph.setPrioritizedDirection(zones[j].node_id, routes[i].routesData[0].linksRoute.back());
		}

	const float pixelPerMeter = 50;

	sf::RenderWindow window;
	window.create(sf::VideoMode(x_full_len * pixelPerMeter + 200, y_full_len * pixelPerMeter), "PedestrianSimulationDemo");
	window.setKeyRepeatEnabled(false);

	sf::Clock clock;
	clock.restart();
	bool m_isSimulating = false, speedUp = false;
	float m_simulationTime = 0.;
	float timeSinceUpdate = 0.;

	const float SIMULATION_STEP = 0.2;
	graph.startSimulation();
	auto destr = graph.getDestribution();

	sf::Text textInfo;
	sf::Font font;
	font.loadFromFile("../Roboto-Black.ttf");
	textInfo.setFont(font);
	textInfo.setCharacterSize(20);
	textInfo.setFillColor(sf::Color::Black);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				switch (event.type)
				{
				case sf::Event::KeyPressed:
					if (event.key.code == sf::Keyboard::C) {
						graph.startSimulation();
						graph.fillWithPeopleEvenly(NodeType::STANDART, fillRatio);
						graph.fillWithPeopleEvenly(NodeType::SOURCE, 0.);
						m_isSimulating = false;
						m_simulationTime = 0.;
					}
					if (event.key.code == sf::Keyboard::Up && !m_isSimulating) {
						fillRatio = std::min(fillRatio + 0.02, 1.);
						graph.fillWithPeopleEvenly(NodeType::STANDART, fillRatio);
					}
					if (event.key.code == sf::Keyboard::Down && !m_isSimulating) {
						fillRatio = std::max(fillRatio - 0.02, 0.);
						graph.fillWithPeopleEvenly(NodeType::STANDART, fillRatio);
					}
					if (event.key.code == sf::Keyboard::Space) {
						m_isSimulating = !m_isSimulating;
					}
					if (event.key.code == sf::Keyboard::Right) {
						speedUp = true;
					}
					if (event.key.code == sf::Keyboard::Left) {
						speedUp = false;
					}
					break;
				default:
					break;
				}
			}

		}
		window.clear();

		float dt = clock.getElapsedTime().asSeconds();
		clock.restart();
		if (m_isSimulating) {
			timeSinceUpdate += dt;
			if (!speedUp && destr.peopleInside > 0) {
				m_simulationTime += dt;
			}
		}

		if (!speedUp && timeSinceUpdate > SIMULATION_STEP && m_isSimulating) {
			timeSinceUpdate -= SIMULATION_STEP;
			graph.makeSimulationStep(SIMULATION_STEP);
		}

		if (speedUp && m_isSimulating && destr.peopleInside > 0) {
			graph.makeSimulationStep(SIMULATION_STEP);
			m_simulationTime += SIMULATION_STEP;
		}

		destr = graph.getDestribution();


		sf::RectangleShape zoneShape{};
		zoneShape.setOutlineThickness(-2);
		textInfo.setFillColor(sf::Color::Black);
		for (size_t i = 0; i < zones.size(); ++i) {
			zoneShape.setOutlineColor(sf::Color(200, 255, 200));
			zoneShape.setPosition(zones[i].x_pos * pixelPerMeter, zones[i].y_pos * pixelPerMeter);
			zoneShape.setSize({ zones[i].x_len * pixelPerMeter, zones[i].y_len * pixelPerMeter });

			int peopleInside = destr.zoneData[zones[i].node_id].peopleInside;
			int maxPeopleInside = zones[i].x_len * zones[i].y_len * 5;

			float fillRatio = static_cast<float>(peopleInside) / static_cast<float>(maxPeopleInside);

			textInfo.setString(std::to_string(peopleInside));
			textInfo.setPosition(zones[i].x_pos * pixelPerMeter, zones[i].y_pos * pixelPerMeter);

			zoneShape.setFillColor(sf::Color(255, 255 - static_cast<float>(255) * fillRatio, 255 - static_cast<float>(255) * fillRatio));

			if (zones[i].x_idx == 4 && zones[i].y_idx == 8) {
				zoneShape.setOutlineColor(sf::Color(0, 0, 255));
			}
			if (zones[i].x_idx == 10 && zones[i].y_idx == 7) {
				zoneShape.setOutlineColor(sf::Color(0, 0, 255));
			}

			window.draw(zoneShape);
			window.draw(textInfo);
		}

		textInfo.setFillColor(sf::Color::White);
		textInfo.setString(std::to_string(destr.peopleInside));
		textInfo.setPosition(x_full_len * pixelPerMeter + 20, 10);
		window.draw(textInfo);

		textInfo.setString(std::to_string(m_simulationTime) + " s.");
		textInfo.setPosition(x_full_len * pixelPerMeter + 20, 30);
		window.draw(textInfo);

		window.display();
	}
}