#include <iostream>
#include "RouteSearchableGraph.h"

int main() {
	RouteSearchableGraph graph;
	auto a1 = graph.createNode(NodeType::STANDART);
	auto a2 = graph.createNode(NodeType::STANDART);
	auto a3 = graph.createNode(NodeType::STANDART);
	auto a4 = graph.createNode(NodeType::STANDART);
	auto a5 = graph.createNode(NodeType::STANDART);

	auto b1 = graph.createNode(NodeType::SOURCE);
	auto b2 = graph.createNode(NodeType::SOURCE);

	graph.createLink(b1, a1, 1);
	graph.createLink(a3, a1, 5);
	graph.createLink(a2, a1, 3);
	graph.createLink(a3, a4, 2);
	graph.createLink(a3, a2, 10);
	graph.createLink(a4, a5, 2);
	graph.createLink(a5, b2, 2);
	graph.createLink(a5, a2, 1);
	graph.createLink(a2, b2, 1);

	auto out = graph.findShortestRoutes(NodeType::SOURCE, NodeType::STANDART);

	return 0;
}