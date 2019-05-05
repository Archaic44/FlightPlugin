#include "flightplugin.h"
#include "bakkesmod\wrappers\ArrayWrapper.h"
#include "bakkesmod\wrappers\CVarManagerWrapper.h"
#include "bakkesmod\wrappers\CVarWrapper.h"
#include "bakkesmod\wrappers\Engine\ActorWrapper.h"
#include "bakkesmod\wrappers\GameEvent\ServerWrapper.h"
#include "bakkesmod\wrappers\GameObject\BallWrapper.h"
#include "bakkesmod\wrappers\GameObject\CarComponent\PrimitiveComponentWrapper.h"
#include "bakkesmod\wrappers\GameObject\CarComponent\VehicleSimWrapper.h"
#include "bakkesmod\wrappers\GameObject\CarWrapper.h"
#include "bakkesmod\wrappers\GameObject\RBActorWrapper.h"
#include "bakkesmod\wrappers\gamewrapper.h"
#include "bakkesmod\wrappers\wrapperstructs.h"
#include "utils\parser.h"

BAKKESMOD_PLUGIN(flightplugin, "Flight plugin", "1.0", PLUGINTYPE_FREEPLAY)
using namespace sp;
void flightplugin::onLoad()
{
	gameWrapper->HookEvent("Function TAGame.Car_TA.SetVehicleInput", bind(&flightplugin::OnSetInput, this));

	liftp = make_shared<float>(0.00385);
	cvarManager->registerCvar("flight_lift", "0.0025", "Lift Power", true, true, 0, true, 1, true).bindTo(liftp);

	dragp = make_shared<float>(0.1);
	cvarManager->registerCvar("flight_drag", "0.1", "You flyin thru mud", true, true, 0, true, 1).bindTo(dragp);

	cmdManager.cvarManager = this->cvarManager;
	cmdManager.gameWrapper = this->gameWrapper;

	cmdManager.addCommands();

	painter.gameWrapper = this->gameWrapper;
	painter.cvarManager = this->cvarManager;

	painter.initDrawables();

	cvarManager->registerNotifier("flight_enabled", [this](std::vector<string> params) {
		if (!gameWrapper->IsInFreeplay())
		{
			cvarManager->log("You need to be in freeplay to use this plugin.");
			return;
		}
		this->flight_enabled = true;
		this->OnSetInput();
		cvarManager->log("Get that air!");
		}, "Starts the Flight mode.", PERMISSION_FREEPLAY);
}
void flightplugin::onUnLoad()
{
	flight_enabled = false;
	return;
}
void flightplugin::OnSetInput()
{
	if (flight_enabled == false) //stops the plugin if flight_enabled isnt true
	{
		return;
	}

/* definitions */
	auto car = gameWrapper->GetGameEventAsServer().GetGameCar();
	RBState rbstate = car.GetRBState();

	Vector loc = rbstate.Location;
	Vector lin = rbstate.LinearVelocity;
	Quat quat = rbstate.Quaternion;
	Vector ang = rbstate.AngularVelocity;
	auto horVel = Vector(lin.X, lin.Y, 0);
	Vector up = quatToUp(quat);
	Vector right = quatToRight(quat);
	Vector fwd = quatToFwd(quat);
	auto linLocalFwd = Vector::dot(lin, fwd);
	auto linLocalRight = Vector::dot(lin, right);
	auto linLocalUp = Vector::dot(lin, up);
	Vector linLocal = Vector(linLocalFwd, linLocalRight, linLocalUp);
	auto lonSpeed = Vector::dot(horVel, fwd);
	auto latSpeed = Vector::dot(horVel, right);

	Vector fwd = quatToFwd(rbstate.Quaternion);



/*Lift*/ //default liftp is 0.00385
	float lift = (*liftp) * lonSpeed;
	Vector liftd = { 0, 0, lift };
	Vector lifted = (lin, up) * lift;
	car.AddVelocity(liftd);

/*Drag*/
//	float drag = ((*dragp * lonSpeed) * -1);
//	Vector dragp = {0, 0, drag};
//	car.AddVelocity(dragp);
}
//START OF Painter.cpp
void Painter::initDrawables()
{
	gameWrapper->RegisterDrawable([this](CanvasWrapper cw) {
		if (!gameWrapper->IsInGame()) {
			return;
		}
		if (cvarManager->getCvar("showHUD").getBoolValue())
		{
			this->drawPanels(cw);}
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
		{drawCarDerivedInfo(cw, car, 20, 55);}
		if (cvarManager->getCvar("showYaw").getBoolValue())
		{drawYawPlane(cw, car, 20, 700, 1);}
	}
}
void Painter::drawStringAt(CanvasWrapper cw, std::string text, int x, int y, Color col)
{
	cw.SetPosition({ x, y });
	cw.SetColor(col.r, col.g, col.b, col.a);
	cw.DrawString(text);
}
void Painter::drawStringAt(CanvasWrapper cw, std::string text, Vector2_ loc, Color col)
{
	drawStringAt(cw, text, loc.X, loc.Y, col);
}
void Painter::drawLine(CanvasWrapper cw, Vector2_ v1, Vector2_ v2)
{
	cw.DrawLine({ v1.X, v1.Y }, { v2.X, v2.Y });
}
void Painter::drawRBStatePanel(CanvasWrapper cw, std::string title, RBState rbstate, int x, int y, bool recording)
{
	Vector loc = rbstate.Location;
	Vector lin = rbstate.LinearVelocity;
	Quat quat = rbstate.Quaternion;
	Vector ang = rbstate.AngularVelocity;
	int marginLeft = 30;
	int marginTop = 20;
	int nameSpacing = 100;
	int vecSpacing = 80;
	int quatSpacing = 120;
	int lineSpacing = 30;
	int width = 620;
	int height = 210;
	cw.SetPosition({ x, y });
	cw.SetColor(COLOR_PANEL);
	cw.FillBox({ width, height });
	cw.SetColor(COLOR_TEXT);

	this->drawStringAt(cw, title, x + marginLeft, y + marginTop);
	int currentLine = marginTop + 50;
	this->drawStringAt(cw, "Location", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(loc.X, 2), x + marginLeft + nameSpacing, y + currentLine);
	this->drawStringAt(cw, sp::to_string(loc.Y, 2), x + marginLeft + nameSpacing + vecSpacing, y + currentLine);
	this->drawStringAt(cw, sp::to_string(loc.Z, 2), x + marginLeft + nameSpacing + vecSpacing * 2, y + currentLine);
	currentLine += lineSpacing;
	this->drawStringAt(cw, "Lin. Velocity", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(lin.X, 2), x + marginLeft + nameSpacing, y + currentLine);
	this->drawStringAt(cw, sp::to_string(lin.Y, 2), x + marginLeft + nameSpacing + vecSpacing, y + currentLine);
	this->drawStringAt(cw, sp::to_string(lin.Z, 2), x + marginLeft + nameSpacing + vecSpacing * 2, y + currentLine);
	currentLine += lineSpacing;
	this->drawStringAt(cw, "Rotation", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string_scientific(quat.W), x + marginLeft + nameSpacing, y + currentLine);
	this->drawStringAt(cw, sp::to_string_scientific(quat.X), x + marginLeft + nameSpacing + quatSpacing, y + currentLine);
	this->drawStringAt(cw, sp::to_string_scientific(quat.Y), x + marginLeft + nameSpacing + quatSpacing * 2, y + currentLine);
	this->drawStringAt(cw, sp::to_string_scientific(quat.Z), x + marginLeft + nameSpacing + quatSpacing * 3, y + currentLine);
	currentLine += lineSpacing;
	this->drawStringAt(cw, "Ang. Velocity", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(ang.X, 4), x + marginLeft + nameSpacing, y + currentLine);
	this->drawStringAt(cw, sp::to_string(ang.Y, 4), x + marginLeft + nameSpacing + vecSpacing, y + currentLine);
	this->drawStringAt(cw, sp::to_string(ang.Z, 4), x + marginLeft + nameSpacing + vecSpacing * 2, y + currentLine);
}
void Painter::drawCarDerivedInfo(CanvasWrapper cw, CarWrapper car, int x, int y)
{
	RBState rbstate = car.GetRBState();
	Vector loc = rbstate.Location;
	Vector lin = rbstate.LinearVelocity; //this lin.magnitude = speed relative to world
	Quat quat = rbstate.Quaternion;
	Vector ang = rbstate.AngularVelocity;
	auto horVel = Vector(lin.X, lin.Y, 0);
	Vector up = quatToUp(quat);
	Vector right = quatToRight(quat);
	Vector fwd = quatToFwd(quat);
	auto linLocalFwd = Vector::dot(lin, fwd);
	auto linLocalRight = Vector::dot(lin, right);
	auto linLocalUp = Vector::dot(lin, up);
	Vector linLocal = Vector(linLocalFwd, linLocalRight, linLocalUp);
	auto lonSpeed = Vector::dot(horVel, fwd); //speed relative to cars forward
	auto latSpeed = Vector::dot(horVel, right); 
//	auto vsim = car.GetVehicleSim();
//	auto wheels = vsim.GetWheels();
	int marginLeft = 10;
	int marginTop = 20;
	int nameSpacing = 100;
	int vecSpacing = 120;
	int quatSpacing = 120;
	int lineSpacing = 30;
	cw.SetPosition({ x, y });
	cw.SetColor(COLOR_PANEL);
	cw.FillBox({ 200, 120 });
	cw.SetColor(COLOR_TEXT);
	cw.SetColor(255, 100, 15, 255);
	this->drawStringAt(cw, "Flight Plugin", x + marginLeft, y + marginTop);
	int currentLine = marginTop + 20;
	cw.SetColor(255, 255, 255, 255);
	this->drawStringAt(cw, "Speed", x + marginLeft, y + currentLine); //speed relative to cars forward
	this->drawStringAt(cw, sp::to_string(lonSpeed, 4), x + marginLeft + nameSpacing, y + currentLine);
	currentLine += lineSpacing;
	this->drawStringAt(cw, "Lat. speed", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(latSpeed, 4), x + marginLeft + nameSpacing, y + currentLine);
	currentLine += lineSpacing;
	this->drawStringAt(cw, "Linear speed", x + marginLeft, y + currentLine); //this lin.magnitude = speed relative to world
	this->drawStringAt(cw, sp::to_string(lin.magnitude(), 4), x + marginLeft + nameSpacing, y + currentLine);

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
	cw.SetPosition({ x, y });
	cw.SetColor(COLOR_PANEL);
	cw.FillBox({ (int)(width * scale), (int)(height * scale) });
	cw.SetColor(COLOR_TEXT);
	Vector2_ center(100, 100);
	Vector2_ axisVer(0, -80);
	Vector2_ axisHor(80, 0);
	Vector2_ orientArrow(0, -90);
	Vector2_ angleLoc(10, 10);
	Vector2_ axisVerLabelLoc(-15, -85);
	Vector2_ axisHorLabelLoc(75, 5);
	drawLine(cw, offset + center * scale, offset + (center + axisVer) * scale);	// x-axis
	drawLine(cw, offset + center * scale, offset + (center + axisHor) * scale);	// y-axis
	float cosTheta = Vector::dot(fwdHor, Vector(1, 0, 0));
	float theta = acos(cosTheta);
	if ((Vector::cross(Vector(1, 0, 0), fwdHor)).Z > 0) { theta = -theta; }
	Vector2_ frontLeft(-42, -73);	// approximate shape/size of octane
	Vector2_ frontRight(42, -73);
	Vector2_ backLeft(-42, 45);
	Vector2_ backRight(42, 45);
	drawLine(cw, offset + (center + frontLeft.rotate(theta)) * scale, offset + (center + frontRight.rotate(theta)) * scale);	// front and back of car
	drawLine(cw, offset + (center + backLeft.rotate(theta)) * scale, offset + (center + backRight.rotate(theta)) * scale);
	drawLine(cw, offset + (center + frontLeft.rotate(theta)) * scale, offset + (center + backLeft.rotate(theta)) * scale);
	drawLine(cw, offset + (center + frontRight.rotate(theta)) * scale, offset + (center + backRight.rotate(theta)) * scale);
	cw.SetColor(255, 255, 255, 128);	// fwd arrow
	drawLine(cw, offset + center * scale, offset + (center + orientArrow.rotate(theta)) * scale);
	cw.SetColor(0, 255, 0, 255);	// velocity arrow
	drawLine(cw, offset + center * scale, offset + (center + velArrow) * scale);
	this->drawStringAt(cw, sp::to_string(theta), offset + angleLoc * scale);
	this->drawStringAt(cw, "x", offset + (center + axisVerLabelLoc) * scale);
	this->drawStringAt(cw, "y", offset + (center + axisHorLabelLoc) * scale);
}
//START OF HelperFunctions.cpp
std::string sp::vector_to_string(Vector v, int prec)
{
	return to_string_decimals(v.X, prec) + ", " + to_string_decimals(v.Y, prec) + ", " + to_string_decimals(v.Z, prec);
}
std::string sp::rot_to_string(Rotator v, int prec)
{
	return to_string_decimals(v.Pitch, prec) + ", " + to_string_decimals(v.Yaw, prec) + ", " + to_string_decimals(v.Roll, prec);
}
std::string sp::quat_to_string(Quat v, int prec)
{
	return to_string_decimals(v.W, prec) + ", " + to_string_decimals(v.X, prec) + ", " + to_string_decimals(v.Y, prec) + ", " + to_string_decimals(v.Z, prec);
}
std::string sp::to_string(float f, int prec)
{
	return to_string_decimals(f, prec);
}
std::string sp::to_string_scientific(float f, int prec, int treshold)
{
	return to_string_decimals_scientific(f, prec, treshold);
}
bool sp::vector_eq(Vector v1, Vector v2)
{
	return (v1.X == v2.X) && (v1.Y == v2.Y) && (v1.Z == v2.Z);
}
bool sp::quat_eq(Quat q1, Quat q2)
{
	return (q1.W == q2.W) && (q1.X == q2.X) && (q1.Y == q2.Y) && (q1.Z == q2.Z);
}
bool sp::rot_eq(Rotator r1, Rotator r2)
{
	return (r1.Pitch == r2.Pitch) && (r1.Yaw == r2.Yaw) && (r1.Roll == r2.Roll);
}
Vector sp::quatToFwd(Quat quat)
{
	// Vector fwd = Vector(1 - 2 * (quat.X*quat.X + quat.Y*quat.Y), 2 * (quat.Y*quat.Z + quat.X*quat.W), -1 * (2 * (quat.X*quat.Z - quat.Y*quat.W)));
	Vector fwd = rotateVectorWithQuat(Vector(1, 0, 0), quat);
	fwd.normalize();
	return fwd;
}
Vector sp::quatToRight(Quat quat)
{
	// Vector right = Vector(2 * (quat.Y*quat.Z - quat.X*quat.W), 1 - 2 * (quat.X*quat.X + quat.Z*quat.Z), -1 * (2 * (quat.X*quat.Y + quat.Z*quat.W)));
	Vector right = rotateVectorWithQuat(Vector(0, 1, 0), quat);
	right.normalize();
	return right;
}
Vector sp::quatToUp(Quat quat)
{
	// Vector up = Vector(2 * (quat.X*quat.Z + quat.Y*quat.W), 2 * (quat.X*quat.Y - quat.Z*quat.W), -1 * (1 - 2 * (quat.Y*quat.Y + quat.Z*quat.Z)));
	Vector up = rotateVectorWithQuat(Vector(0, 0, 1), quat);
	up.normalize();
	return up;
}
Vector sp::rotateVectorWithQuat(Vector v, Quat q)
{
	Quat p;
	p.W = 0;
	p.X = v.X;
	p.Y = v.Y;
	p.Z = v.Z;
	Quat result = (q * p) * q.conjugate();
	return Vector(result.X, result.Y, result.Z);
}
//START OF CommandsManager.cpp
void CommandsManager::addCommands()
{
	cvarManager->registerCvar("recordEveryTick", "0");
	cvarManager->registerCvar("showHUD", "1");
	cvarManager->registerCvar("showCarDerivedInfo", "1");
	cvarManager->registerCvar("showYaw", "1");
	cvarManager->registerNotifier("setCarRBLocation", [this](std::vector<string> commands) {
		if (commands.size() > 3)
		{
			float x = get_safe_float(commands.at(1));
			float y = get_safe_float(commands.at(2));
			float z = get_safe_float(commands.at(3));
			ServerWrapper tutorial = gameWrapper->GetGameEventAsServer();
			CarWrapper gameCar = tutorial.GetGameCar();
			VehicleWrapper vehicle = (VehicleWrapper)gameCar;
			RBActorWrapper rbActor = (RBActorWrapper)vehicle;
			ActorWrapper actor = (ActorWrapper)rbActor;
			auto rbstate = rbActor.GetRBState();
			auto loc = rbstate.Location;
			loc.X = x;
			loc.Y = y;
			loc.Z = z;
			rbstate.Location = loc;
			rbActor.SetPhysicsState(rbstate);
			std::string logString = "car RB location set to " + vector_to_string(loc);
			logger.log(logString);
		}
		}, "", 0);
	cvarManager->registerNotifier("setCarRBLinearVelocity", [this](std::vector<string> commands) {
		if (commands.size() > 3)
		{
			float x = get_safe_float(commands.at(1));
			float y = get_safe_float(commands.at(2));
			float z = get_safe_float(commands.at(3));
			ServerWrapper tutorial = gameWrapper->GetGameEventAsServer();
			CarWrapper gameCar = tutorial.GetGameCar();
			VehicleWrapper vehicle = (VehicleWrapper)gameCar;
			RBActorWrapper rbActor = (RBActorWrapper)vehicle;
			ActorWrapper actor = (ActorWrapper)rbActor;
			auto rbstate = rbActor.GetRBState();
			auto vel = rbstate.LinearVelocity;
			vel.X = x;
			vel.Y = y;
			vel.Z = z;
			rbstate.LinearVelocity = vel;
			rbActor.SetPhysicsState(rbstate);
			std::string logString = "car RB linear velocity set to " + vector_to_string(vel, 2);
			logger.log(logString);
		}
		}, "", 0);
	cvarManager->registerNotifier("setCarRBAngularVelocity", [this](std::vector<string> commands) {
		if (commands.size() > 3)
		{
			float x = get_safe_float(commands.at(1));
			float y = get_safe_float(commands.at(2));
			float z = get_safe_float(commands.at(3));
			ServerWrapper tutorial = gameWrapper->GetGameEventAsServer();
			CarWrapper gameCar = tutorial.GetGameCar();
			VehicleWrapper vehicle = (VehicleWrapper)gameCar;
			RBActorWrapper rbActor = (RBActorWrapper)vehicle;
			ActorWrapper actor = (ActorWrapper)rbActor;
			auto rbstate = rbActor.GetRBState();
			auto vel = rbstate.AngularVelocity;
			vel.X = x;
			vel.Y = y;
			vel.Z = z;
			rbstate.AngularVelocity = vel;
			rbActor.SetPhysicsState(rbstate);
			std::string logString = "car RB angular velocity set to " + vector_to_string(vel, 4);
			logger.log(logString);
		}
		}, "", 0);
	cvarManager->registerNotifier("setCarRBQuaternion", [this](std::vector<string> commands) {
		if (commands.size() > 4)
		{
			float w = get_safe_float(commands.at(1));
			float x = get_safe_float(commands.at(2));
			float y = get_safe_float(commands.at(3));
			float z = get_safe_float(commands.at(4));
			ServerWrapper tutorial = gameWrapper->GetGameEventAsServer();
			CarWrapper gameCar = tutorial.GetGameCar();
			VehicleWrapper vehicle = (VehicleWrapper)gameCar;
			RBActorWrapper rbActor = (RBActorWrapper)vehicle;
			ActorWrapper actor = (ActorWrapper)rbActor;
			auto rbstate = rbActor.GetRBState();
			auto quat = rbstate.Quaternion;
			quat.W = w;
			quat.X = x;
			quat.Y = y;
			quat.Z = z;
			rbstate.Quaternion = quat;
			rbActor.SetPhysicsState(rbstate);
			std::string logString = "car RB quaternion set to " + quat_to_string(quat);
			logger.log(logString);
		}
		}, "", 0);
	cvarManager->registerNotifier("resetCar", [this](std::vector<string> commands) {
		if (commands.size() > 0)
		{
			ServerWrapper tutorial = gameWrapper->GetGameEventAsServer();
			CarWrapper gameCar = tutorial.GetGameCar();
			auto rbstate = gameCar.GetRBState();
			rbstate.LinearVelocity = Vector(0, 0, 0);
			rbstate.Quaternion.W = 1;
			rbstate.Quaternion.X = 0;
			rbstate.Quaternion.Y = 0;
			rbstate.Quaternion.Z = 0;
			rbstate.AngularVelocity = Vector(0, 0, 0);
			gameCar.SetPhysicsState(rbstate);
			std::string logString = "Car has been reset";
			logger.log(logString);
		}
		}, "", 0);
	cvarManager->registerNotifier("slideRight", [this](std::vector<string> commands) {
		if (commands.size() > 1)
		{
			std::string s = commands.at(1);
			float f = get_safe_float(s);
			ServerWrapper tutorial = gameWrapper->GetGameEventAsServer();
			CarWrapper gameCar = tutorial.GetGameCar();
			auto rbstate = gameCar.GetRBState();
			auto quatRB = rbstate.Quaternion;
			Vector right = quatToRight(quatRB);
			right.normalize();
			auto slide = Vector(right.X * f, right.Y * f, right.Z * f);
			gameCar.AddVelocity(slide);
			std::string logString = "Velocity increased by " + vector_to_string(slide);
			logger.log(logString);
		}
		}, "", 0);
	cvarManager->registerNotifier("setMass", [this](std::vector<string> commands) {
		if (commands.size() > 1)
		{
			std::string s = commands.at(1);
			float f = get_safe_float(s);
			ServerWrapper tutorial = gameWrapper->GetGameEventAsServer();
			CarWrapper gameCar = tutorial.GetGameCar();
			gameCar.SetMass(f);
			std::string logString = "RB mass set to " + to_string(s);
			logger.log(logString);
		}
		}, "", 0);
	cvarManager->registerNotifier("setOnGround", [this](std::vector<string> commands) {
		if (commands.size() > 1)
		{
			std::string s = commands.at(1);
			bool b = get_safe_bool(s);
			ServerWrapper tutorial = gameWrapper->GetGameEventAsServer();
			CarWrapper gameCar = tutorial.GetGameCar();
			gameCar.SetbOnGround((unsigned long)b);
			std::string logString = "bOnGround set to " + to_string(s);
			logger.log(logString);
		}
		}, "", 0);
	cvarManager->registerNotifier("setUseAckermannSteering", [this](std::vector<string> commands) {
		if (commands.size() > 1)
		{
			std::string s = commands.at(1);
			bool b = get_safe_bool(s);
			ServerWrapper tutorial = gameWrapper->GetGameEventAsServer();
			CarWrapper gameCar = tutorial.GetGameCar();
			gameCar.GetVehicleSim().SetbUseAckermannSteering(b);
			std::string logString = "bUseAckermannSteering set to " + to_string(s);
			logger.log(logString);
		}
		}, "", 0);
	cvarManager->registerNotifier("getUseAckermannSteering", [this](std::vector<string> commands) {
		ServerWrapper tutorial = gameWrapper->GetGameEventAsServer();
		CarWrapper gameCar = tutorial.GetGameCar();
		auto b = gameCar.GetVehicleSim().GetbUseAckermannSteering();
		std::string logString = "bUseAckermannSteering = " + to_string(b);
		logger.log(logString);
		}, "", 0);
	cvarManager->registerNotifier("setDriveTorque", [this](std::vector<string> commands) {
		if (commands.size() > 1)
		{
			auto s = commands.at(1);
			float f = get_safe_float(s);
			ServerWrapper training = gameWrapper->GetGameEventAsServer();
			CarWrapper car = training.GetGameCar();
			auto vehicleSim = car.GetVehicleSim();
			vehicleSim.SetDriveTorque(f);
			auto logString = "drive torque set to: " + std::to_string(vehicleSim.GetDriveTorque());
			logger.log(logString);
		}
		}, "", 0);
	cvarManager->registerNotifier("getDriveTorque", [this](std::vector<string> commands) {
		ServerWrapper training = gameWrapper->GetGameEventAsServer();
		CarWrapper car = training.GetGameCar();
		auto vehicleSim = car.GetVehicleSim();
		auto logString = "drive torque: " + std::to_string(vehicleSim.GetDriveTorque());
		logger.log(logString);
		}, "", 0);
	cvarManager->registerNotifier("setBrakeTorque", [this](std::vector<string> commands) {
		if (commands.size() > 1)
		{
			auto s = commands.at(1);
			float f = get_safe_float(s);
			ServerWrapper training = gameWrapper->GetGameEventAsServer();
			CarWrapper car = training.GetGameCar();
			auto vehicleSim = car.GetVehicleSim();
			vehicleSim.SetBrakeTorque(f);
			auto logString = "brake torque set to: " + std::to_string(vehicleSim.GetBrakeTorque());
			logger.log(logString);
		}
		}, "", 0);
	cvarManager->registerNotifier("getBrakeTorque", [this](std::vector<string> commands) {
		ServerWrapper training = gameWrapper->GetGameEventAsServer();
		CarWrapper car = training.GetGameCar();
		auto vehicleSim = car.GetVehicleSim();
		auto logString = "brake torque: " + std::to_string(vehicleSim.GetBrakeTorque());
		logger.log(logString);
		}, "", 0);
	cvarManager->registerNotifier("setStopThreshold", [this](std::vector<string> commands) {
		if (commands.size() > 1)
		{
			auto s = commands.at(1);
			float f = get_safe_float(s);
			ServerWrapper training = gameWrapper->GetGameEventAsServer();
			CarWrapper car = training.GetGameCar();
			auto vehicleSim = car.GetVehicleSim();
			vehicleSim.SetStopThreshold(f);
			auto logString = "stop threshold set to: " + std::to_string(vehicleSim.GetStopThreshold());
			logger.log(logString);
		}
		}, "", 0);
	cvarManager->registerNotifier("getStopThreshold", [this](std::vector<string> commands) {
		ServerWrapper training = gameWrapper->GetGameEventAsServer();
		CarWrapper car = training.GetGameCar();
		auto vehicleSim = car.GetVehicleSim();
		auto logString = "stop threshold: " + std::to_string(vehicleSim.GetStopThreshold());
		logger.log(logString);
		}, "", 0);
	cvarManager->registerNotifier("setIdleBrakeFactor", [this](std::vector<string> commands) {
		if (commands.size() > 1)
		{
			auto s = commands.at(1);
			float f = get_safe_float(s);
			ServerWrapper training = gameWrapper->GetGameEventAsServer();
			CarWrapper car = training.GetGameCar();
			auto vehicleSim = car.GetVehicleSim();
			vehicleSim.SetIdleBrakeFactor(f);
			auto logString = "idle brake factor set to: " + std::to_string(vehicleSim.GetIdleBrakeFactor());
			logger.log(logString);
		}
		}, "", 0);
	cvarManager->registerNotifier("getIdleBrakeFactor", [this](std::vector<string> commands) {
		ServerWrapper training = gameWrapper->GetGameEventAsServer();
		CarWrapper car = training.GetGameCar();
		auto vehicleSim = car.GetVehicleSim();
		auto logString = "idle brake factor: " + std::to_string(vehicleSim.GetIdleBrakeFactor());
		logger.log(logString);
		}, "", 0);
	cvarManager->registerNotifier("setOppositeBrakeFactor", [this](std::vector<string> commands) {
		if (commands.size() > 1)
		{
			auto s = commands.at(1);
			float f = get_safe_float(s);
			ServerWrapper training = gameWrapper->GetGameEventAsServer();
			CarWrapper car = training.GetGameCar();
			auto vehicleSim = car.GetVehicleSim();
			vehicleSim.SetOppositeBrakeFactor(f);
			auto logString = "opposite brake factor set to: " + std::to_string(vehicleSim.GetOppositeBrakeFactor());
			logger.log(logString);
		}
		}, "", 0);
	cvarManager->registerNotifier("getOppositeBrakeFactor", [this](std::vector<string> commands) {
		ServerWrapper training = gameWrapper->GetGameEventAsServer();
		CarWrapper car = training.GetGameCar();
		auto vehicleSim = car.GetVehicleSim();
		auto logString = "opposite brake factor: " + std::to_string(vehicleSim.GetOppositeBrakeFactor());
		logger.log(logString);
		}, "", 0);
	cvarManager->registerNotifier("getSuspensionStiffness", [this](std::vector<string> commands) {
		ServerWrapper training = gameWrapper->GetGameEventAsServer();
		CarWrapper car = training.GetGameCar();
		auto vehicleSim = car.GetVehicleSim();
		ArrayWrapper<WheelWrapper> wheels = vehicleSim.GetWheels();
		if (wheels.Count() >= 4)
		{
			for (int i = 0; i < wheels.Count(); i++)
			{
				auto logString = "suspension stiffness for wheel " + std::to_string(i) + ": " + std::to_string(wheels.Get(i).GetSuspensionStiffness());
				logger.log(logString);
			}
		}
		}, "", 0);
	cvarManager->registerNotifier("getSuspensionDampingCompression", [this](std::vector<string> commands) {
		ServerWrapper training = gameWrapper->GetGameEventAsServer();
		CarWrapper car = training.GetGameCar();
		auto vehicleSim = car.GetVehicleSim();
		ArrayWrapper<WheelWrapper> wheels = vehicleSim.GetWheels();
		if (wheels.Count() >= 4)
		{
			for (int i = 0; i < wheels.Count(); i++)
			{
				auto logString = "suspension damping compression for wheel " + std::to_string(i) + ": " + std::to_string(wheels.Get(i).GetSuspensionDampingCompression());
				logger.log(logString);
			}
		}
		}, "", 0);
	cvarManager->registerNotifier("getSuspensionDampingRelaxation", [this](std::vector<string> commands) {
		ServerWrapper training = gameWrapper->GetGameEventAsServer();
		CarWrapper car = training.GetGameCar();
		auto vehicleSim = car.GetVehicleSim();
		ArrayWrapper<WheelWrapper> wheels = vehicleSim.GetWheels();
		if (wheels.Count() >= 4)
		{
			for (int i = 0; i < wheels.Count(); i++)
			{
				auto logString = "suspension damping relaxation for wheel " + std::to_string(i) + ": " + std::to_string(wheels.Get(i).GetSuspensionDampingRelaxation());
				logger.log(logString);
			}
		}
		}, "", 0);
	cvarManager->registerNotifier("getSuspensionTravel", [this](std::vector<string> commands) {
		ServerWrapper training = gameWrapper->GetGameEventAsServer();
		CarWrapper car = training.GetGameCar();
		auto vehicleSim = car.GetVehicleSim();
		ArrayWrapper<WheelWrapper> wheels = vehicleSim.GetWheels();
		if (wheels.Count() >= 4)
		{
			for (int i = 0; i < wheels.Count(); i++)
			{
				auto logString = "suspension travel for wheel " + std::to_string(i) + ": " + std::to_string(wheels.Get(i).GetSuspensionTravel());
				logger.log(logString);
			}
		}
		}, "", 0);
	cvarManager->registerNotifier("getSuspensionMaxRaise", [this](std::vector<string> commands) {
		ServerWrapper training = gameWrapper->GetGameEventAsServer();
		CarWrapper car = training.GetGameCar();
		auto vehicleSim = car.GetVehicleSim();
		ArrayWrapper<WheelWrapper> wheels = vehicleSim.GetWheels();
		if (wheels.Count() >= 4)
		{
			for (int i = 0; i < wheels.Count(); i++)
			{
				auto logString = "suspension max raise for wheel " + std::to_string(i) + ": " + std::to_string(wheels.Get(i).GetSuspensionMaxRaise());
				logger.log(logString);
			}
		}
		}, "", 0);
	cvarManager->registerNotifier("setSpinSpeedDecayRate", [this](std::vector<string> commands) {
		if (commands.size() > 1)
		{
			auto s = commands.at(1);
			float f = get_safe_float(s);
			ServerWrapper training = gameWrapper->GetGameEventAsServer();
			CarWrapper car = training.GetGameCar();
			auto vehicleSim = car.GetVehicleSim();
			ArrayWrapper<WheelWrapper> wheels = vehicleSim.GetWheels();
			if (wheels.Count() >= 4)
			{
				for (int i = 0; i < wheels.Count(); i++)
				{
					wheels.Get(i).SetSpinSpeedDecayRate(f);
				}
				auto logString = "spinspeed decay rate for all wheels set to: " + std::to_string(wheels.Get(0).GetSpinSpeedDecayRate());
				logger.log(logString);
			}
		}
		}, "", 0);
	cvarManager->registerNotifier("getSpinSpeedDecayRate", [this](std::vector<string> commands) {
		ServerWrapper training = gameWrapper->GetGameEventAsServer();
		CarWrapper car = training.GetGameCar();
		auto vehicleSim = car.GetVehicleSim();
		ArrayWrapper<WheelWrapper> wheels = vehicleSim.GetWheels();
		if (wheels.Count() >= 4)
		{
			for (int i = 0; i < wheels.Count(); i++)
			{
				auto logString = "spinspeed decay rate for wheel " + std::to_string(i) + ": " + std::to_string(wheels.Get(i).GetSpinSpeedDecayRate());
				logger.log(logString);
			}
		}
		}, "", 0);
	cvarManager->registerNotifier("setSpinSpeed", [this](std::vector<string> commands) {
		if (commands.size() > 2)
		{
			auto s = commands.at(1);
			auto s2 = commands.at(2);
			float f = get_safe_float(s2);
			auto i = get_safe_int(s);
			if (i < 0 || i > 3) {
				return;
			}
			ServerWrapper training = gameWrapper->GetGameEventAsServer();
			CarWrapper car = training.GetGameCar();
			auto vehicleSim = car.GetVehicleSim();
			ArrayWrapper<WheelWrapper> wheels = vehicleSim.GetWheels();
			wheels.Get(i).SetSpinSpeed(f);
			auto logString = "spinspeed for wheel " + to_string(i) + " set to: " + std::to_string(wheels.Get(i).GetSpinSpeed());
			logger.log(logString);
		}
		}, "", 0);
	cvarManager->registerNotifier("getSpinSpeed", [this](std::vector<string> commands) {
		ServerWrapper training = gameWrapper->GetGameEventAsServer();
		CarWrapper car = training.GetGameCar();
		auto vehicleSim = car.GetVehicleSim();
		ArrayWrapper<WheelWrapper> wheels = vehicleSim.GetWheels();
		if (wheels.Count() >= 4)
		{
			for (int i = 0; i < wheels.Count(); i++)
			{
				auto logString = "spinspeed for wheel " + std::to_string(i) + ": " + std::to_string(wheels.Get(i).GetSpinSpeed());
				logger.log(logString);
			}
		}
		}, "", 0);
	cvarManager->registerNotifier("getSteer", [this](std::vector<string> commands) {
		ServerWrapper training = gameWrapper->GetGameEventAsServer();
		CarWrapper car = training.GetGameCar();
		auto vehicleSim = car.GetVehicleSim();
		ArrayWrapper<WheelWrapper> wheels = vehicleSim.GetWheels();
		if (wheels.Count() >= 4)
		{
			for (int i = 0; i < wheels.Count(); i++)
			{
				auto logString = "steer for wheel " + std::to_string(i) + ": " + std::to_string(wheels.Get(i).GetSteer2());
				logger.log(logString);
			}
		}
		}, "", 0);
	cvarManager->registerNotifier("getSuspensionDistance", [this](std::vector<string> commands) {
		ServerWrapper training = gameWrapper->GetGameEventAsServer();
		CarWrapper car = training.GetGameCar();
		auto vehicleSim = car.GetVehicleSim();
		ArrayWrapper<WheelWrapper> wheels = vehicleSim.GetWheels();
		if (wheels.Count() >= 4)
		{
			for (int i = 0; i < wheels.Count(); i++)
			{
				auto logString = "suspension distance for wheel " + std::to_string(i) + ": " + std::to_string(wheels.Get(i).GetSuspensionDistance());
				logger.log(logString);
			}
		}
		}, "", 0);
	cvarManager->registerNotifier("getRefWheelLocation", [this](std::vector<string> commands) {
		ServerWrapper training = gameWrapper->GetGameEventAsServer();
		CarWrapper car = training.GetGameCar();
		auto vehicleSim = car.GetVehicleSim();
		ArrayWrapper<WheelWrapper> wheels = vehicleSim.GetWheels();
		if (wheels.Count() >= 4)
		{
			for (int i = 0; i < wheels.Count(); i++)
			{
				auto logString = "ref wheel location wheel " + std::to_string(i) + ": " + vector_to_string(wheels.Get(i).GetRefWheelLocation());
				logger.log(logString);
			}
		}
		}, "", 0);
}
//START OF Logger.cpp
void Logger::log(std::string s)
{
	cvarManager->log(s);
}
void Logger::log(Vector v)
{
	cvarManager->log(vector_to_string(v));
}
void Logger::log(Rotator r)
{
	cvarManager->log(rot_to_string(r));
}
void Logger::log(float f)
{
	cvarManager->log(sp::to_string(f));
}
void Logger::log(bool b)
{
	cvarManager->log(to_string(b));
}
void Logger::log(int i)
{
	cvarManager->log(to_string(i));
}
void Logger::log(unsigned long l)
{
	cvarManager->log(to_string(l));
}