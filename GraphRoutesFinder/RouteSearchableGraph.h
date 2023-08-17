#pragma once
#include <memory>
#include <vector>
#include <set>
#include <map>

#include "NodeType.h"

using GraphNodeID = unsigned;
using GraphLinkID = unsigned;

const GraphNodeID UnsetGraphNodeID = GraphNodeID();
const GraphLinkID UnsetGraphLinkID = GraphLinkID();

// Type for link's weight in weighted graph
using WeightType = int;

// Describes one route from start to finish
struct GraphRoute 
{
	GraphNodeID start = UnsetGraphNodeID, finish = UnsetGraphNodeID;
	WeightType routeWeight = WeightType();

	std::vector<GraphNodeID> nodeRoute;
	std::vector<GraphLinkID> linksRoute;
};

// Describes all found routes to node
struct GraphRoutesToNode
{
	GraphNodeID node = UnsetGraphNodeID;
	bool isRoutesExist = false;
	WeightType routeWeight = WeightType();
	std::vector<GraphRoute> routesData;
};

class RouteSearchableGraph final 
{
public:
	RouteSearchableGraph();
	~RouteSearchableGraph() = default;

	// creates Node with type, returns its ID
	GraphNodeID createNode(NodeType type = NodeType::NO_TYPE);
	// creates link between 2 nodes with some weight. Can be directed or undirected
	GraphLinkID createLink(GraphNodeID from, GraphNodeID to, WeightType weight = WeightType(), bool isDirected = false);

	// remove node from graph. Also removes all links to/from this node
	void removeNode(GraphNodeID node);
	void removeLink(GraphLinkID link);

	bool isNodeExists(GraphNodeID node);
	bool isLinkExists(GraphLinkID link);
	bool isLinkExists(GraphNodeID from, GraphNodeID to);

	// returns true if at least 1 node of type exists
	bool isNodeOfTypeExists(NodeType type);

	// returns array of nodes whitch doesn't have links to any other nodes
	std::vector<GraphNodeID> getExternalNodes();

	// returns true if graph is weakly connected (if all links are undirected it's equal to connected)
	bool isConnected();

	// returns array of all shortest paths
	GraphRoutesToNode findShortestRoutes(NodeType startNodesType, GraphNodeID finishNode);
	// array for every finish node of array of all shortest paths
	std::vector<GraphRoutesToNode> findShortestRoutes(NodeType startNodesType, const std::vector<GraphNodeID>& finishNode);
	// array for every finish node of array of all shortest paths
	std::vector<GraphRoutesToNode> findShortestRoutes(NodeType startNodesType, NodeType finishNodesType);
	// returns array of all shortest paths. Includes only routes with weight less than maxWeight
	GraphRoutesToNode findShortestRoutes(NodeType startNodesType, GraphNodeID finishNode, WeightType maxWeight);
	// array for every finish node of array of all shortest paths. Includes only routes with weight less than maxWeight
	std::vector<GraphRoutesToNode> findShortestRoutes(NodeType startNodesType, const std::vector<GraphNodeID>& finishNode, WeightType maxWeight);
	// array for every finish node of array of all shortest paths. Includes only routes with weight less than maxWeight
	std::vector<GraphRoutesToNode> findShortestRoutes(NodeType startNodesType, NodeType finishNodesType, WeightType maxWeight);
private:
	struct GraphNode;
	struct GraphLink;

	using node_ptr = std::shared_ptr<GraphNode>;
	using link_ptr = std::shared_ptr<GraphLink>;

	struct GraphNode 
	{
		GraphNodeID ID = UnsetGraphNodeID;
		NodeType type = NodeType::NO_TYPE;

		std::set<link_ptr> inputLinks, outputLinks;

		struct searchingNodeInfo {
			GraphRoutesToNode shortestRoutes;
			bool isVisited = false;
		} searchingInfo;
	};

	struct GraphLink 
	{
		GraphLinkID ID = UnsetGraphLinkID;

		node_ptr from, to;

		bool isDirected = true;
		WeightType weight = WeightType();
	};


	// Maps of all nodes and links of a graph
	std::map<GraphNodeID, node_ptr> m_Nodes;
	std::map<GraphLinkID, link_ptr> m_Links;

	// Set of nodes of every type
	std::map<NodeType, std::set<node_ptr>> m_typedNodes;

	GraphNodeID m_nextFreeGraphNodeID;
	GraphLinkID m_nextFreeGraphLinkID;

	void removeLink(link_ptr link);

	void clearSearchingInfo();

	// starts in start, find route to every node in nodesToEndIn
	void forwardDFS(node_ptr start, WeightType maxWeight, bool findOverweightRoutes);

	// starts in finish, find route from some node from nodesToStartIn to finish
	void Dijkstra(node_ptr start, WeightType maxWeight, bool findOverweightRoutes);
};