#include "Button.h"
#include "raylib\raymath.h"
#include <thread>
#include <iostream>

#include <chrono>
using namespace std::chrono_literals;

void RadioButton::select()
{
	selected = 1;
}

void RadioButton::deselect()
{
	selected = 0;
}

RadioButton::RadioButton(const Vector2& pos, const float& r)
{
	position = pos;
	radius = r;
}

void RadioButton::draw()
{
	DrawCircle(position.x, position.y, radius, RAYWHITE);
	if (selected)
	{
		DrawCircle(position.x, position.y, radius * 0.5, BLACK);
	}
}

bool RadioButton::isSelected()
{
	return selected;
}

Knob::Knob(const Vector2& pos, const float& r, float* var, const Color& bodyColor, const Color& lineColor)
	:position(pos), radius(r), variable(var), angle(DEG2RAD* (*variable - 270)), color(bodyColor), lineColor(lineColor)
{
	//std::cout << var << std::endl << variable << std::endl << std::endl;
}

void Knob::update()
{
	angle = (DEG2RAD * (*variable - 270));
}

void Knob::deselect()
{
	isSelected = false;
	color = BLACK;
	lineColor = RAYWHITE;
}

void Knob::grab()
{
	if (this->isPressed())
	{
		this->select();
		//Vector2 oldMousePos = GetMousePosition();
		while (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			this->rotate(-GetMouseDelta().y * 0.01);
			std::this_thread::sleep_for(1ms);
		}
		this->deselect();
	}
}

void Knob::draw()
{
	DrawCircle(position.x, position.y, radius, color);
	DrawLine(position.x, position.y, position.x + cos(angle) * radius, position.y + sin(angle) * radius, lineColor);
}

void Knob::select()
{
	isSelected = true;
	color = SKYBLUE;
	lineColor = BLACK;
}

void Knob::rotate(const float& rotAngle)
{
	float newValue = *variable + rotAngle;
	if (newValue < 0)
		*variable = 0;
	else
		*variable = *variable + rotAngle;
	update();
	//std::cout << variable << " " << *variable << std::endl;

}

RectangularButton::RectangularButton(const Vector2& pos, const float& wd, const float& ht, const Color& clr)
	:Button(pos), width(wd), height(ht)
{
	position.x = pos.x;
	position.y = pos.y;

	color.r = clr.r;
	color.g = clr.g;
	color.b = clr.b;
	color.a = clr.a;

	fontSize = height * 0.8;

}



Button::Button(const Vector2& pos)
{
	position.x = pos.x;
	position.y = pos.y;
}

void RectangularButton::draw()
{
	//std::cout << position.x << " " << position.y << " " << width << " " << height << std::endl;
	DrawRectangle(position.x, position.y, width, height, color);
	DrawText(label.c_str(), position.x, position.y, fontSize, BLACK);
}

bool RectangularButton::isPressed()
{
	Vector2 mousePos = GetMousePosition();
	return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
		mousePos.x > position.x &&
		mousePos.x < position.x + width &&
		mousePos.y > position.y &&
		mousePos.y < position.y + height;
}
