#pragma once

#include "raylib.h"

class ScreenElement
{
protected:
	float widthRatio;
	float heightRatio;
	float xRatio;
	float yRatio;
public:
	ScreenElement() : xRatio(0.0), yRatio(0.0), widthRatio(0.0), heightRatio(0.0) {}
	ScreenElement(float xRatio, float yRatio, float widthRatio, float heightRatio) : xRatio(xRatio), yRatio(yRatio), widthRatio(widthRatio), heightRatio(heightRatio) {}
	float getWidth() { return GetScreenWidth() * widthRatio; }
	float getHeight() { return GetScreenHeight() * heightRatio; }
	float getX() { return GetScreenWidth() * xRatio; }
	float getY() { return GetScreenHeight() * yRatio; }
	virtual void draw() {}
	virtual void keyEvents() {}
	virtual void update() { draw(); keyEvents(); }
};