#pragma once
#include <cstdint>
#include <string>
#include "raylib\raylib.h"

class Node
{
private:
	std::string label;
	unsigned int number;
	Vector2 pos;
	Color oldColor = RED;
	Color color = oldColor;
	bool selected = false;
public:
	Node() {};
	Node(const int& number, const std::string &lbl, const Vector2& position, const Color& c = RED);
	Node(const Node& node);
	void select();
	void deselect();
	inline Vector2 getPos()const { return pos; }
	inline unsigned int getNumber() const { return number; }
	inline Color getColor() const { return color; }
	inline Color getOldColor() const { return oldColor; }
	void setColor(const Color &color);
	void setOldColor(const Color& color);
	void setPos(const Vector2 &position);
	void draw();

	static int radius;
	static int fontSize;
	//static void setRadius(const int& r) { radius = r; }
};