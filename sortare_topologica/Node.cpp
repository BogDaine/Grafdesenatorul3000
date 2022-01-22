#include "Node.h"
#include "string"

Node::Node(const int& number, const std::string& lbl, const Vector2& position, const Color& c)
	:number(number), label(lbl), pos(position), oldColor(c), color(oldColor)
{
}

Node::Node(const Node& node)
{
	number = node.getNumber();
	pos = node.getPos();
	color = node.getColor();
}

void Node::select()
{
	selected = true;
}

void Node::deselect()
{
	selected = false;
}

void Node::setColor(const Color &color)
{
	this->color = color;
}

void Node::setOldColor(const Color& color)
{
	oldColor = color;
}

void Node::setPos(const Vector2& position)
{
	pos = position;
}

void Node::draw()
{
	
	DrawCircle(pos.x, pos.y, Node::radius, selected?BLUE:color);


	DrawText(label.c_str(), pos.x - 7, pos.y - 7, Node::fontSize, BLACK);
	
}