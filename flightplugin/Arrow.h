#pragma once
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include <vector>

class Arrow
{

private:
	float length, width, height, x_offset, y_offset, z_offset;
	void computePoints();  //  fill in Vector of the 8 rectangular prism points
	std::vector<Vector> points;
	CarWrapper* car;

public:
	Arrow(float length, float width, float height, float x_offset, float y_offset, float z_offset, CarWrapper* car);
	void getPoints(std::vector<Vector>& pts);
	~Arrow();
};

