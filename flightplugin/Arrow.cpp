#include "Arrow.h"

Arrow::Arrow(float length, float width, float height, float x_offset, float y_offset, float z_offset, CarWrapper* car) {
	this->length = length;
	this->width = width;
	this->height = height;
	this->x_offset = x_offset;
	this->y_offset = y_offset;
	this->z_offset = z_offset;
	this->car = car;

	computePoints();
}

void Arrow::computePoints() {
	points.clear();
	if (this->car != nullptr) {
		float vel = car->GetRBState().LinearVelocity.magnitude();
		float maxV = car->GetMaxDriveForwardSpeed();
		points.push_back(Vector(length * vel / maxV, 0, 0));
	}
	else
	{
		points.push_back(Vector(0, 0, 0));
	}
	points.push_back(Vector(-length, 0, 0));


	for (int i = 0; i < 2; i++) {
		points[i].X += x_offset;
		points[i].Z += y_offset; // plugin uses Y axis as up
		points[i].Y += z_offset; // Z axis of the game is up
	}
}

void Arrow::getPoints(std::vector<Vector>& pts)
{
	pts.clear();
	for (int i = 0; i < 2; i++)
		pts.push_back(points[i]);
}

Arrow::~Arrow()
{
}
