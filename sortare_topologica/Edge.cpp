#include "Edge.h"

Edge::Edge(Node* node1, Node* node2)
	:node1(node1), node2(node2)
{
}

Edge::Edge(std::pair<Node*, Node*> nodes)
{
	node1 = nodes.first;
	node2 = nodes.second;
}

void Edge::draw()
{
	auto node1Pos = node1->getPos();
	auto node2Pos = node2->getPos();

	float deltaX = node2Pos.x - node1Pos.x;
	float deltaY = node2Pos.y - node1Pos.y;
	float length = sqrt(deltaX * deltaX + deltaY * deltaY);
	
	float rL = Node::radius / length;

	float x1 = node1Pos.x + deltaX * rL;
	float y1 = node1Pos.y + deltaY * rL;
	
	float x2 = node2Pos.x - deltaX * rL;
	float y2 = node2Pos.y - deltaY * rL;

	//https://stackoverflow.com/questions/34931902/how-to-draw-a-line-that-connects-two-circle-outlines-without-trig-functions

	DrawLine(x1, y1, x2, y2, color);
}

bool Edge::operator==(const Edge& edge) const
{
	if (node1 == edge.node1 && node2 == edge.node2)
		return true;
	return false;
}
