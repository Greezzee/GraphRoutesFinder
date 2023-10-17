#include <iostream>
#include "GraphTestGenerator.h"
#include <SFML/Graphics.hpp>

#include "../BuildingToGraphConverter/VoronoiWrapper.h"

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
/*
using graphs::NodeType;

struct GraphZone {
	int x_idx, y_idx;

	float x_pos, y_pos;
	float x_len, y_len;

	graphs::GraphNodeID node_id = graphs::UnsetGraphNodeID;
};

int main() {
	graphs::PedestrianSimulatorGraphFD graph;

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
	}

	auto exitA = graph.createNode(NodeType::SOURCE, 9999.);
	graph.setNodeExitCapacity(exitA, 30.5);
	auto exitB = graph.createNode(NodeType::SOURCE, 9999.);
	graph.setNodeExitCapacity(exitB, 20.5);

	graph.setExitType(NodeType::SOURCE);

	for (int i = 0; i < zones.size(); ++i) {
		for (int j = 0; j < zones.size(); ++j) {

			if (zones[i].x_idx == zones[j].x_idx && zones[i].x_idx % 2 == 0 && zones[i].y_idx == zones[j].y_idx + 1) {

				if (!(zones[i].x_idx == 8 && zones[i].y_idx == 5)) {
					graph.createLink(zones[i].node_id, zones[j].node_id, zones[i].x_len, (zones[i].y_len + zones[j].y_len) / 2.f);
				}

			}

			if (zones[i].y_idx == zones[j].y_idx && zones[i].y_idx % 2 == 0 && zones[i].x_idx == zones[j].x_idx + 1) {
				if (zones[i].x_idx != 6 && zones[j].x_idx != 6) {
					graph.createLink(zones[i].node_id, zones[j].node_id, zones[i].y_len, (zones[i].x_len + zones[j].x_len) / 2.f);
				}
			}

			if (zones[i].x_idx == 6 && zones[j].x_idx == 5) {
				if (zones[j].y_idx == 0 && zones[i].y_idx == 0 ||
					zones[j].y_idx == 2 && zones[i].y_idx == 2 ||
					zones[j].y_idx == 4 && zones[i].y_idx == 3 ||
					zones[j].y_idx == 6 && zones[i].y_idx == 5 ||
					zones[j].y_idx == 8 && zones[i].y_idx == 7) {

					graph.createLink(zones[i].node_id, zones[j].node_id, zones[j].y_len, (zones[i].x_len + zones[j].x_len) / 2.f);
				}
			}

			if (zones[i].x_idx == 6 && zones[j].x_idx == 7) {
				if (zones[j].y_idx == 0 && zones[i].y_idx == 0 ||
					zones[j].y_idx == 2 && zones[i].y_idx == 2 ||
					zones[j].y_idx == 4 && zones[i].y_idx == 3 ||
					zones[j].y_idx == 6 && zones[i].y_idx == 4 ||
					zones[j].y_idx == 8 && zones[i].y_idx == 5 ||
					zones[j].y_idx == 10 && zones[i].y_idx == 7) {

					graph.createLink(zones[i].node_id, zones[j].node_id, zones[j].y_len, (zones[i].x_len + zones[j].x_len) / 2.f);
				}
			}
		}

		if (zones[i].x_idx == 4 && zones[i].y_idx == 8) {
			graph.createLink(zones[i].node_id, exitB, 1.5, (zones[i].x_len) / 2.f);
		}
		if (zones[i].x_idx == 10 && zones[i].y_idx == 7) {
			graph.createLink(zones[i].node_id, exitA, 2, (zones[i].x_len) / 2.f);
		}
	}

	graph.setClosestExitAsPrioritizedDirection(NodeType::STANDART);

	const float pixelPerMeter = 50;

	sf::RenderWindow window;
	window.create(sf::VideoMode(x_full_len * pixelPerMeter + 200, y_full_len * pixelPerMeter), "PedestrianSimulationDemo");
	window.setKeyRepeatEnabled(false);

	sf::Clock clock;
	clock.restart();
	bool m_isSimulating = false, speedUp = false;
	float m_simulationTime = 0.;
	float timeSinceUpdate = 0.;

	const float SIMULATION_STEP = 0.2f;
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
						fillRatio = std::min(fillRatio + 0.02f, 1.f);
						graph.fillWithPeopleEvenly(NodeType::STANDART, fillRatio);
					}
					if (event.key.code == sf::Keyboard::Down && !m_isSimulating) {
						fillRatio = std::max(fillRatio - 0.02f, 0.f);
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
*/

#include "../BuildingToGraphConverter/Wall.h"

constexpr double WALL_STEP = 100;
constexpr double GRID_STEP = 100;

int main() {


	double x_full_len = 1000;
	double y_full_len = 1000;

	sf::RenderWindow window;
	window.create(sf::VideoMode(x_full_len, y_full_len), "PedestrianSimulationDemo");
	window.setKeyRepeatEnabled(false);

	sf::CircleShape circle;
	circle.setFillColor(sf::Color::White);
	circle.setRadius(4.f);
	circle.setOrigin(2.f, 2.f);

	sf::RectangleShape rect;
	rect.setFillColor(sf::Color::White);
	rect.setOrigin(0, -0.5);

	window.setFramerateLimit(60);

	std::vector<voronoi::Wall<double>> walls;
	std::vector<voronoi::Vector2D<double>> voronoiPoints;
	std::vector<voronoi::Vector2D<double>> gridPoints;

	voronoi::VoronoiWrapper<voronoi::Vector2D<double>> null_wrapper;
	auto output = null_wrapper.constructVoronoi();

	bool isActiveLastWall = false;

	sf::Clock clock;
	clock.restart();

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			switch (event.type)
			{
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::C && isActiveLastWall) {
					walls.back().setIsClosed(true);
				}

				if (event.key.code == sf::Keyboard::R) {
					walls.clear();
					isActiveLastWall = false;
					voronoiPoints.clear();
					output.clear();
					gridPoints.clear();
				}

				if (event.key.code == sf::Keyboard::Escape) {
					isActiveLastWall = false;
					voronoiPoints.clear();
					output.clear();
					gridPoints.clear();
				}

				if (event.key.code == sf::Keyboard::X) {
					voronoiPoints.clear();
					gridPoints.clear();
					for (auto& wall : walls) {
						wall.addInnerEquidistantPoints(WALL_STEP);
						auto newPoints = wall.generateAdditionalPointsForVoronoi(WALL_STEP / 2.);
						voronoiPoints.insert(voronoiPoints.end(), newPoints.begin(), newPoints.end());
					}

					double x = WALL_STEP, y = WALL_STEP;

					while (x < x_full_len) {

						while (y < y_full_len) {

							double minDistance = WALL_STEP * 2;

							for (auto point : voronoiPoints) {
								double distance = (point - voronoi::Vector2D<double>{x, y}).lenght();
								minDistance = std::min(minDistance, distance);
							}
							if (minDistance > WALL_STEP || voronoiPoints.empty())
								gridPoints.push_back({ x, y });

							y += WALL_STEP;
						}

						x += WALL_STEP;
						y = WALL_STEP;
					}

				}

				if (event.key.code == sf::Keyboard::Z) {
					voronoi::VoronoiWrapper<voronoi::Vector2D<double>> wrapper;
					wrapper.setBoundingBox(voronoi::Vector2D<double>{ 0., 0. }, voronoi::Vector2D<double>{ x_full_len, y_full_len });
					auto newVector = voronoiPoints;
					newVector.insert(newVector.end(), gridPoints.begin(), gridPoints.end());
					wrapper.setPoints(newVector);
					output = wrapper.constructVoronoi();
				}

				break;
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left && clock.getElapsedTime().asMilliseconds() > 100) {
					clock.restart();
					if (isActiveLastWall) {
						voronoi::Vector2D<double> newPoint;
						newPoint.x = static_cast<double>(event.mouseButton.x);
						newPoint.y = static_cast<double>(event.mouseButton.y);
						walls.back().addPoint(newPoint);
					}
					else {
						isActiveLastWall = true;
						walls.push_back(voronoi::Wall<double>());
						voronoi::Vector2D<double> newPoint;
						newPoint.x = static_cast<double>(event.mouseButton.x);
						newPoint.y = static_cast<double>(event.mouseButton.y);
						walls.back().addPoint(newPoint);
					}
				}
				if (event.mouseButton.button == sf::Mouse::Right) {
					isActiveLastWall = false;
				}
				break;
			default:
				break;
			}
		}
		window.clear();

		for (auto site : output) {
			sf::Color color = sf::Color(rand() % 255, rand() % 255, rand() % 255);
			for (auto edge : site.edges) {
				rect.setFillColor(sf::Color::Green);
				rect.setSize(sf::Vector2f(edge.edge.lenght(), 1));
				rect.setRotation(edge.edge.getAngle() / PI * 180);
				rect.setPosition(sf::Vector2f(edge.edge.offset.x, edge.edge.offset.y));
				window.draw(rect);
			}
		}

		for (auto wall : walls) {
			auto pointList = wall.getPoints();

			for (auto point : pointList) {
				circle.setFillColor(sf::Color::White);
				circle.setPosition(point.x, point.y);
				window.draw(circle);
			}

			for (auto point = pointList.begin(), point_e = std::prev(pointList.end()); point != point_e; ++point) {

				voronoi::Segment2D<voronoi::Vector2D<double>> edge;
				edge.x = (*std::next(point) - *point).x;
				edge.y = (*std::next(point) - *point).y;
				edge.offset = *point;

				rect.setFillColor(sf::Color::White);
				rect.setSize(sf::Vector2f(edge.lenght(), 2));
				rect.setRotation(edge.getAngle() / PI * 180);
				rect.setPosition(sf::Vector2f(edge.offset.x, edge.offset.y));
				window.draw(rect);
			}

			if (wall.isClosed()) {
				voronoi::Segment2D<voronoi::Vector2D<double>> edge;
				edge.x = (pointList.front() - pointList.back()).x;
				edge.y = (pointList.front() - pointList.back()).y;
				edge.offset = pointList.back();

				rect.setFillColor(sf::Color::White);
				rect.setSize(sf::Vector2f(edge.lenght(), 2));
				rect.setRotation(edge.getAngle() / PI * 180);
				rect.setPosition(sf::Vector2f(edge.offset.x, edge.offset.y));
				window.draw(rect);
			}
		}

		for (auto point : voronoiPoints) {
			circle.setFillColor(sf::Color::Green);
			circle.setPosition(point.x, point.y);
			window.draw(circle);
		}

		for (auto point : gridPoints) {
			circle.setFillColor(sf::Color::Blue);
			circle.setPosition(point.x, point.y);
			window.draw(circle);
		}

		window.display();
	}
    return 0;
}