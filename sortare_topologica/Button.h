#pragma once
#include "raylib\raylib.h"
#include "raylib\raymath.h"

#include "Drawable.h"

#include <string>

class Button : public Drawable
{
protected:
	Vector2 position = { 0,0 };
public:
	Button() {};
	Button(const Vector2 & pos);
	inline Vector2 getPosition() { return position; }
	virtual void draw() = 0;
	virtual bool isPressed() = 0;
};

class RadioButton : public Button
{
private:
	float radius = 0;
	bool selected = 0;

public:
	void select();
	void deselect();
	RadioButton(const Vector2 & pos, const float &r);
	void draw() override;
	bool isSelected();
	bool isPressed() override { return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && Vector2Distance(GetMousePosition(), this->position) < radius; }

	

};

class RectangularButton : public Button
{
protected:
	float width, height;
	std::string label;
	int fontSize;
	Color color;
public:
	RectangularButton(const Vector2& pos, const float& wd, const float& ht, const Color& clr = LIGHTGRAY);
	void setFontSize(const float& fs) { fontSize = int(fs); }
	void setLabel(const std::string& lb) { label = lb; }
	void setColor(const Color& clr) { color = clr; }
	void draw() override;
	bool isPressed() override;
};

class Knob : public Button
{
private:
	Vector2 position = { 0,0 };
	float radius = 0;
	Color color;
	Color lineColor;
	float *variable;
	float angle;

	bool isSelected = 0;


	void update();

public:
	Knob(const Vector2& pos, const float& r, float *var, const Color &bodyColor = BLACK, const Color &lineColor = RAYWHITE);
	void draw();

	void rotate(const float &rotAngle);

	void grab();

	inline Vector2 getPosition() { return position; }
	void setPosition(const Vector2& pos) { position = pos; }
	inline float getValue() { return *variable; }
	inline float *getValuePointer() { return variable; }

	void select();
	void deselect();
	bool isPressed() override{ return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && Vector2Distance(GetMousePosition(), this->position) < radius; }
};