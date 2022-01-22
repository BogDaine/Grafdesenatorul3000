#pragma once
#include "Node.h"
#include <utility>
#include "raylib\raylib.h"

class Edge
{
private:
	Node* node1, * node2;
	Color color = BLACK;
public:
	Edge(Node* node1, Node* node2);
	Edge(std::pair<Node*, Node*>);
	void draw();
	bool operator ==(const Edge& edge) const;
	inline std::pair<Node*, Node*> const getNodes() { return std::make_pair(node1, node2); }
};