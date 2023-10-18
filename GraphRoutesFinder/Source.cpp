#include <iostream>
#include "RouteSearchableGraph.h"
#include <iostream>

using namespace graphs;

int main() {
	RouteSearchableGraph<> graph;
	auto a1 = graph.createNode(NodeType::STANDART);
	auto a2 = graph.createNode(NodeType::STANDART);
	auto a3 = graph.createNode(NodeType::STANDART);
	auto a4 = graph.createNode(NodeType::STANDART);
	auto a5 = graph.createNode(NodeType::STANDART);

	auto b1 = graph.createNode(NodeType::SOURCE);
	auto b2 = graph.createNode(NodeType::SOURCE);

	graph.createLink(b1, a1, 1);
	graph.createLink(a3, a1, 5);
	auto link_a1a2 = graph.createLink(a2, a1, 3);
	graph.createLink(a3, a4, 2);
	graph.createLink(a3, a2, 10);
	auto link_a4a5 = graph.createLink(a4, a5, 2);
	graph.createLink(a5, b2, 2);
	graph.createLink(a5, a2, 1);
	graph.createLink(a2, b2, 1);

	auto linkData = graph.getConnectingNodes(link_a1a2);
	auto outputLinks = graph.getOutputLinks(a1);
	auto inputLinks = graph.getInputLinks(a1);
	auto links = graph.getLinks(a1);

	auto out = graph.findShortestRoutes(NodeType::SOURCE, NodeType::STANDART);

	bool data1 = graph.isNodeExists(a2);
	bool data2 = graph.isLinkExists(b1, a1);
	bool data3 = graph.isLinkExists(a3, a5);
	bool data4 = graph.isConnected();
	bool data5 = graph.isNodeOfTypeExists(NodeType::SOURCE);
	bool data6 = graph.isNodeOfTypeExists(NodeType::NO_TYPE);
	bool data7 = graph.isLinkExists(link_a1a2);

	graph.removeLink(link_a1a2);
	bool data8 = graph.isLinkExists(link_a1a2);
	bool data9 = graph.isLinkExists(a1, a2);
	
	graph.removeNode(a3);

	out = graph.findShortestRoutes(NodeType::SOURCE, NodeType::STANDART);

	bool data10 = graph.isConnected();
	auto data11 = graph.getExternalNodes();
	bool data12 = graph.isNodeExists(a3);

	graph.removeLink(link_a4a5);

	auto data13 = graph.getExternalNodes();

	auto dataComponents = graph.getWeaklyConnectedComponents();
	auto dataComponent = graph.getWeaklyConnectedComponentContainsNode(a2);

	auto data14 = graph.getUnreachableNodes(NodeType::SOURCE);

	return 0;
}
