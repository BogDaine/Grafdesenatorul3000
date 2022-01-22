#pragma once
#include "Node.h"
#include "Edge.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "Config.h"
#include "Drawable.h"

#include <iostream>

enum class graph_types
{
	ORIENTED,
	UNORIENTED
};

enum graph_drawing_modes
{
	NORMAL,
	GROUPED_COLOR,
	GROUPED_NODE
};

class Graph : public Drawable
{
private:					
	std::vector<std::tuple<unsigned int, unsigned int, unsigned int, unsigned int, bool /*is special needs*/>> edges;
	std::vector<std::vector<unsigned int>> CC;								 // connected components
	std::vector<std::vector<unsigned int>>SCC;								 // strongly connected components
	std::vector<std::vector<unsigned int>>*group;
	std::vector<Node*> nodes;

	bool oriented = 0;
	bool updatedCC = false;
	bool updatedSCC = false;
	bool flow = false;


	unsigned int drawingMode = graph_drawing_modes::NORMAL;
	bool hidden = false;

	unsigned int numberOfNodes = 0;
	unsigned int numberOfPaths = 0;
	unsigned int numberOfCycles = 0;


	unsigned int circleRadius;


	void generateRandomPath(const unsigned int& start, const unsigned int& finish);
	void generateRandomCycle(const unsigned int& start);

	void drawGrouped_color(std::vector < std::vector<unsigned int>>& groups);
	void drawGrouped_nodes();

public:
	Graph(const bool &oriented = 0);

	bool hasFlow() const;

	void resetGraph();

	//random graph constructor
	Graph(const unsigned int &nNodes, const unsigned int &nPaths, const unsigned int &nCycles, const bool &oriented = 0, const bool& fl = 0, const bool& resizeNodes = true, const unsigned int& cRadius = 380);
	//
	void drawEdges();
	void drawNodes();
	void draw() override;
	
	void setHidden(const bool& val) { hidden = val; }
	const bool isHidden()const { return hidden; }

	std::vector<std::vector<unsigned int>>& getSCC() { return SCC; }
	std::vector<std::vector<unsigned int>>& getCC() { return CC; }

	void group_Color(std::vector < std::vector<unsigned int>>& groups);
	void group_Nodes(std::vector < std::vector<unsigned int>>& groups);
	void setGroup(std::vector<std::vector<unsigned int>>* g) { group = g; }
	void resetColors();

	unsigned int** getMatrix(unsigned int &n);
	inline unsigned int getNumberOfNodes() { return numberOfNodes; }
	inline std::vector<std::vector<unsigned int>> getAdjacencyList();

	void dumpMatrixToFile(std::string fileName);
	void dumpAdjacencyListToFile(std::string fileName);

	void drawEdge(const unsigned int&, const unsigned int&, const unsigned int& index);
	void addNode();
	void addNodeNextNode(const Vector2 &position);
	bool addEdge(Node *node1, Node *node2, const unsigned int& cap = 0);
	void printEdges();
	Node* checkNodeCollision(const Vector2 &position, const Node* except = NULL);
	Node* checkNodeOverlap(const Vector2 &position, const Node* except = NULL);
	bool checkEdgeExistOriented(const unsigned int& node1, const unsigned int& node2);
	bool checkEdgeExistUnoriented(const unsigned int& node1, const unsigned int& node2);
	bool checkEdgeExistOriented(Node* node1, Node* node2);
	bool checkEdgeExistUnoriented(Node* node1, Node* node2);

	inline bool isOriented() { return oriented; }

	void setDrawingMode(const unsigned int& mode) { drawingMode = mode; }

	int FindRoot();

	unsigned int maxFlow(const unsigned int& s, const unsigned int& t);

	static void DFS_Root(const unsigned int&, std::vector<std::vector<unsigned int>>&, std::vector<bool>&, const bool& oriented, bool&);

	static int StronglyConnectedComponents_tarjan(std::vector<std::vector<unsigned int>>& AdjacancyList,
		std::vector<unsigned int>& lowlink);
	static std::vector<std::vector<unsigned int>> TarjanOutputToGroup(std::vector<unsigned int>& lowlink, const unsigned int& SCCCount);

	static std::vector<std::vector<unsigned int>> AdjTransposed(std::vector<std::vector<unsigned int>>& AdjacencyList);

	static int StronglyConnectedComponents_kosaraju(std::vector < std::vector<unsigned int>>& AdjacencyList, std::vector<std::vector<unsigned int>>& result);
	
	static int ConnectedComponents(std::vector<std::vector<unsigned int>>& AdjacencyList, std::vector<std::vector<unsigned int>>& result);
	static int TopSort(std::vector<std::vector<unsigned int>>& AdjacancyList, std::vector<int>& result);
	static int HasCycles(std::vector<std::vector<unsigned int>>& AdjacancyList, const bool& oriented = true);
};