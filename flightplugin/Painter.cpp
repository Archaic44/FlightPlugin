#include "bakkesmod\wrappers\GameEvent\TutorialWrapper.h"
#include "bakkesmod\wrappers\GameObject\CarWrapper.h"
#include "bakkesmod\wrappers\GameObject\CarComponent\VehicleSimWrapper.h"
#include "bakkesmod\wrappers\arraywrapper.h"
#include "bakkesmod\wrappers\wrapperstructs.h"
#include "Painter.h"
#include "HelperFunctions.h"
#include "flightplugin.h"

using namespace sp;

void Painter::initDrawables()
{
	gameWrapper->RegisterDrawable([this](CanvasWrapper cw) {
		if (!gameWrapper->IsInGame()) {
			return; 
		}
		if (cvarManager->getCvar("showHUD").getBoolValue())
		{
			this->drawPanels(cw);
		}
		});
}
void Painter::drawPanels(CanvasWrapper cw)
{
	auto players = gameWrapper->GetGameEventAsServer().GetCars();
	if (players.Count() > 0)
	{
		CarWrapper car = players.Get(0);
		RBState rbstate = car.GetRBState();
		if (cvarManager->getCvar("showCarDerivedInfo").getBoolValue())
		{
			drawCarDerivedInfo(cw, car, 20, 750);
		}
		if (cvarManager->getCvar("showYaw").getBoolValue())
		{
			drawYawPlane(cw, car, 20, 550, 1);
		}
		if (cvarManager->getCvar("showSlider").getBoolValue())
		{
			drawSliderValues(cw, car, 360, 550);
		}
	}
}

void Painter::drawStringAt(CanvasWrapper cw, std::string text, int x, int y, Color col)
{
	Vector2 vec = { x,y };
	cw.SetPosition(vec);
	cw.SetColor(col.r, col.g, col.b, col.a);
	cw.DrawString(text);
}

void Painter::drawStringAt(CanvasWrapper cw, std::string text, Vector2_ loc, Color col)
{
	drawStringAt(cw, text, loc.X, loc.Y, col);
}

void Painter::drawLine(CanvasWrapper cw, Vector2_ v1, Vector2_ v2)
{
	Vector2 _v1 = { v1.X, v1.Y };
	Vector2 _v2 = { v2.X, v2.Y };
	cw.DrawLine(_v1, _v2);
}

void Painter::drawCarDerivedInfo(CanvasWrapper cw, CarWrapper car, int x, int y)
{
	// Car Info
	RBState rbstate = car.GetRBState();

	Vector loc = rbstate.Location;
	Vector lin = rbstate.LinearVelocity; //this is world velocities
	Quat quat = rbstate.Quaternion; // Quaternion describing the car's current basis
	Vector ang = rbstate.AngularVelocity;

	// Car Frame
	// Car's directions
	Vector up = quatToUp(quat);  // Car's Up direction relative to world's xyz
	Vector right = quatToRight(quat); // Car's Right vector relative to world's xyz
	Vector fwd = quatToFwd(quat); // Car's forward vector relative to world's xyz

	// Ground Speeds
	Vector groundVel = Vector(lin.X, lin.Y, 0);
	auto airspeed = abs(Vector::dot(groundVel, fwd)); // speed relative to car forward direction

	// Car's Velocities
	auto linLocalFwd = Vector::dot(lin, fwd);
	auto linLocalRight = Vector::dot(lin, right);
	auto linLocalUp = Vector::dot(lin, up);
	Vector linLocal = Vector(linLocalFwd, linLocalRight, linLocalUp);
	float percm = *shared->percm;
	int speedmode = *shared->speedmode;
	int marginLeft = 10;
	int marginTop = 20;
	int titleSpacing = 120;
	int nameSpacing = 130;
	int vecSpacing = 70;
	int quatSpacing = 120;
	int lineSpacing = 30;
	Vector2 pos = { x,y };
	cw.SetPosition(pos);
	cw.SetColor(COLOR_PANEL);
	Vector2 box = { 250, 160 }; //Flight Plugin box size width, length
	cw.FillBox(box);
	cw.SetColor(COLOR_TEXT);
	cw.SetColor(205, 155, 15, 255);
	this->drawStringAt(cw, "Flight Plugin", x + titleSpacing, y + marginTop);
	int currentLine = marginTop + 20;
	cw.SetColor(255, 255, 255, 255);
	//this->drawStringAt(cw, "Car Location", x + marginLeft, y + currentLine); // Car location in world
	//this->drawStringAt(cw, sp::vector_to_string(loc, 2), x + marginLeft + nameSpacing, y + currentLine);
	//currentLine += lineSpacing;
	this->drawStringAt(cw, "Ground Speed", x + marginLeft, y + currentLine); //this lin.magnitude = speed relative to world
	this->drawStringAt(cw, sp::to_string(lin.magnitude(), 4), x + marginLeft + nameSpacing, y + currentLine);
	currentLine += lineSpacing;
	this->drawStringAt(cw, "Air Speed", x + marginLeft, y + currentLine); //speed relative to cars forward
	this->drawStringAt(cw, sp::to_string(airspeed, 4), x + marginLeft + nameSpacing, y + currentLine);
	currentLine += lineSpacing;
	this->drawStringAt(cw, "% Max speed", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(percm, 4), x + marginLeft + nameSpacing, y + currentLine);
	currentLine += lineSpacing;
	this->drawStringAt(cw, "Speedmode", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(speedmode, 4), x + marginLeft + nameSpacing, y + currentLine);
}

void Painter::drawSliderValues(CanvasWrapper cw, CarWrapper car, int x, int y)
{
	flightplugin* s = shared;
	Vector car_dimensions = Vector(*s->length, *s->width, *s->height);
	Vector drag = Vector(*s->x_drag, *s->y_drag, *s->z_drag);
	Vector stabilization = Vector(*s->pitch_scalar, *s->roll_scalar, *s->yaw_scalar);
	float lift =*s->lift;
	float boost = *s->boost;
	float speed = (2300 * *s->max_speed);
	float throttle = *s->throttle;
	int marginLeft = 10;
	int marginTop = 20;
	int titleSpacing = 90;
	int nameSpacing = 130;
	int vecSpacing = 70;
	int quatSpacing = 120;
	int lineSpacing = 30;
	Vector2 monitor_size = cw.GetSize();
	Vector2 pos = { monitor_size.X - x, monitor_size.Y-y };
	cw.SetPosition(pos);
	cw.SetColor(COLOR_PANEL);
	Vector2 box = { 300, 280 };
	cw.FillBox(box);
	cw.SetColor(COLOR_TEXT);
	cw.SetColor(205, 155, 15, 255);
	this->drawStringAt(cw, "Slider Values", pos.X + titleSpacing, pos.Y + marginTop);
	int currentLine = marginTop + 20;
	cw.SetColor(255, 255, 255, 255);
	this->drawStringAt(cw, "Max Speed: ", pos.X + marginLeft, pos.Y + currentLine);
	this->drawStringAt(cw, sp::to_string(speed, 3), pos.X + marginLeft + nameSpacing, pos.Y + currentLine);
	currentLine += lineSpacing;
	this->drawStringAt(cw, "Boost Power: ", pos.X + marginLeft, pos.Y + currentLine);
	this->drawStringAt(cw, sp::to_string(boost, 3), pos.X + marginLeft + nameSpacing, pos.Y + currentLine);
	currentLine += lineSpacing;
	this->drawStringAt(cw, "Air Density: ", pos.X + marginLeft, pos.Y + currentLine);
	this->drawStringAt(cw, sp::to_string(*shared->rho, 3), pos.X + marginLeft + nameSpacing, pos.Y + currentLine);
	currentLine += lineSpacing;
	this->drawStringAt(cw, "Car LWH: ", pos.X + marginLeft, pos.Y + currentLine);
	this->drawStringAt(cw, sp::vector_to_string(car_dimensions, 3), pos.X + marginLeft + nameSpacing, pos.Y + currentLine);
	currentLine += lineSpacing;
	this->drawStringAt(cw, "Drag: ", pos.X + marginLeft, pos.Y + currentLine);
	this->drawStringAt(cw, sp::vector_to_string(drag, 3), pos.X + marginLeft + nameSpacing, pos.Y + currentLine);
	currentLine += lineSpacing;
	this->drawStringAt(cw, "Stabilization: ", pos.X + marginLeft, pos.Y + currentLine);
	this->drawStringAt(cw, sp::vector_to_string(stabilization, 3), pos.X + marginLeft + nameSpacing, pos.Y + currentLine);
	currentLine += lineSpacing;
	this->drawStringAt(cw, "Lift: ", pos.X + marginLeft, pos.Y + currentLine);
	this->drawStringAt(cw, sp::to_string(lift, 3), pos.X + marginLeft + nameSpacing, pos.Y + currentLine);
	currentLine += lineSpacing;
	this->drawStringAt(cw, "Throttle: ", pos.X + marginLeft, pos.Y + currentLine);
	this->drawStringAt(cw, sp::to_string(throttle, 3), pos.X + marginLeft + nameSpacing, pos.Y + currentLine);
}

Vector Painter::Rotate(Vector aVec, double roll, double yaw, double pitch)
{
	float sx = sin(roll);
	float cx = cos(roll);
	float sy = sin(yaw);
	float cy = cos(yaw);
	float sz = sin(pitch);
	float cz = cos(pitch);

	aVec = Vector(aVec.X, aVec.Y * cx - aVec.Z * sx, aVec.Y * sx + aVec.Z * cx);  //2  roll?
	aVec = Vector(aVec.X * cz - aVec.Y * sz, aVec.X * sz + aVec.Y * cz, aVec.Z); //1   pitch?
	aVec = Vector(aVec.X * cy + aVec.Z * sy, aVec.Y, -aVec.X * sy + aVec.Z * cy);  //3  yaw?

	// ugly fix to change coordinates to Unreal's axes
	float tmp = aVec.Z;
	aVec.Z = aVec.Y;
	aVec.Y = tmp;
	return aVec;
}

void Painter::drawYawPlane(CanvasWrapper cw, CarWrapper car, int x, int y, float scale)
{
	RBState rbstate = car.GetRBState();
	Vector loc = rbstate.Location;
	Vector lin = rbstate.LinearVelocity;
	Quat quat = rbstate.Quaternion;
	Vector ang = rbstate.AngularVelocity;
	Vector up = quatToUp(quat);
	Vector right = quatToRight(quat);
	Vector fwd = quatToFwd(quat);
	Vector fwdHor(fwd.X, fwd.Y, 0);
	fwdHor.normalize();
	Vector velHor(lin.X, lin.Y, 0);
	if (velHor.magnitude() > 0)
	{
		float fraction = (velHor.magnitude() / 2300) * 100;
		velHor.normalize();
		velHor = velHor * fraction; // scale vector to range 0-100 to fit it in the panel
	}
	Vector2_ velArrow(velHor.Y, -velHor.X);
	Vector2_ offset(x, y);
	int width = 200;
	int height = 200;
	Vector2 pos = { x,y };
	cw.SetPosition(pos);
	cw.SetColor(COLOR_PANEL);
	Vector2 box = { width*scale, height*scale };
	cw.FillBox(box);
	cw.SetColor(COLOR_TEXT);
	Vector2_ center(100, 100);
	Vector2_ axisVer(0, -80);
	Vector2_ axisHor(80, 0);
	Vector2_ orientArrow(0, -90);
	Vector2_ angleLoc(10, 10);
	Vector2_ axisVerLabelLoc(-15, -85);
	Vector2_ axisHorLabelLoc(75, 5);
	//drawLine(cw, offset + center * scale, offset + (center + axisVer) * scale);	// x-axis
	//drawLine(cw, offset + center * scale, offset + (center + axisHor) * scale);	// y-axis
	float cosTheta = Vector::dot(fwdHor, Vector(1, 0, 0));
	float theta = acos(cosTheta);
	if ((Vector::cross(Vector(1, 0, 0), fwdHor)).Z > 0) { theta = -theta; }
	//Vector2_ frontLeft(-42, -73);	// approximate shape/size of octane
	//Vector2_ frontRight(42, -73);
	//Vector2_ backLeft(-42, 45);
	//Vector2_ backRight(42, 45);
	//drawLine(cw, offset + (center + frontLeft.rotate(theta)) * scale, offset + (center + frontRight.rotate(theta)) * scale);	// front and back of car
	//drawLine(cw, offset + (center + backLeft.rotate(theta)) * scale, offset + (center + backRight.rotate(theta)) * scale);
	//drawLine(cw, offset + (center + frontLeft.rotate(theta)) * scale, offset + (center + backLeft.rotate(theta)) * scale);
	//drawLine(cw, offset + (center + frontRight.rotate(theta)) * scale, offset + (center + backRight.rotate(theta)) * scale);
	cw.SetColor(255, 255, 255, 128);	// fwd arrow
	drawLine(cw, offset + center * scale, offset + (center + orientArrow.rotate(theta)) * scale);
	
	cw.SetColor(0, 255, 0, 255);	// velocity arrow
//	drawLine(cw, offset + center * scale, offset + (center + velArrow) * scale);
	drawLine(cw, offset + center * scale, offset + (center + velArrow));

	this->drawStringAt(cw, sp::to_string(theta), offset + angleLoc * scale);
	this->drawStringAt(cw, "x", offset + (center + axisVerLabelLoc) * scale);
	this->drawStringAt(cw, "y", offset + (center + axisHorLabelLoc) * scale);
}
void Painter::drawInputPanel(CanvasWrapper cw, CarWrapper car, int x, int y)
{
	ControllerInput ci = car.GetInput();

	int marginLeft = 30;
	int marginTop = 20;

	int nameSpacing = 125;
	int vecSpacing = 120;
	int quatSpacing = 120;

	int lineSpacing = 30;

	Vector2 pos = { x,y };
	cw.SetPosition(pos);
	cw.SetColor(COLOR_PANEL);
	Vector2 box = { 420, 280 };
	cw.SetColor(COLOR_TEXT);

	this->drawStringAt(cw, "Player input", x + marginLeft, y + marginTop);

	int currentLine = marginTop + 50;

	this->drawStringAt(cw, "Throttle", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(ci.Throttle), x + marginLeft + nameSpacing, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Steer", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(ci.Steer), x + marginLeft + nameSpacing, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Pitch", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(ci.Pitch), x + marginLeft + nameSpacing, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Yaw", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(ci.Yaw), x + marginLeft + nameSpacing, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Roll", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(ci.Roll), x + marginLeft + nameSpacing, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Handbrake", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(ci.Handbrake), x + marginLeft + nameSpacing, y + currentLine);

	// second column
	currentLine = marginTop + 50;
	marginLeft += 200;

	this->drawStringAt(cw, "Jump", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(ci.Jump), x + marginLeft + nameSpacing, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Jumped", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(ci.Jumped), x + marginLeft + nameSpacing, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Activate boost", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(ci.ActivateBoost), x + marginLeft + nameSpacing, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Holding boost", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(ci.HoldingBoost), x + marginLeft + nameSpacing, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Dodge forward", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(ci.DodgeForward), x + marginLeft + nameSpacing, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Dodge strafe", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(ci.DodgeStrafe), x + marginLeft + nameSpacing, y + currentLine);
}