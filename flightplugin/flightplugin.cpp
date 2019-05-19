#define LINMATH_H //Conflicts with linmath.h if we done declare this here

#include "flightplugin.h"
#include "bakkesmod\wrappers\ArrayWrapper.h"
#include "bakkesmod\wrappers\GameObject/CarWrapper.h"
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
#include "bakkesmod\wrappers\GameEvent\TutorialWrapper.h"
#include "bakkesmod/wrappers/arraywrapper.h"
#include "utils\parser.h"


BAKKESMOD_PLUGIN(flightplugin, "Flight plugin", "1.0.0", PLUGINTYPE_FREEPLAY)

using namespace sp;

void flightplugin::onLoad()
{
	gameWrapper->HookEvent("Function TAGame.Car_TA.SetVehicleInput", bind(&flightplugin::OnSetInput, this));

	liftmag = make_shared<float>();
	cvarManager->registerCvar("flight_lift", "0.415", "Lift Power", true, true, 0, true, 1, true).bindTo(liftmag);

	dragmag = make_shared<float>();
	cvarManager->registerCvar("flight_drag", "0.1", "You flyin thru mud", true, true, 0, true, 10).bindTo(dragmag);

	cmdManager.cvarManager = this->cvarManager;
	cmdManager.gameWrapper = this->gameWrapper;

	liftmag = make_shared<float>();

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



	// Lift 
	float lifty = (*liftmag) * lonSpeed;
	Vector lift = (lin, up) * lifty;
	car.AddForce(lift, 1);

	// Drag - Value is linear right now, maybe magnify it based on angle of attack in the future
	float dragy = (*dragmag * lin.magnitude);
	Vector drag = (dragy);
	car.AddVelocity(drag);
}
void flightplugin::drawStringAt(CanvasWrapper cw, std::string text, int x, int y, Color col)
{
	cw.SetPosition({ x, y });
	cw.SetColor(col.r, col.g, col.b, col.a);
	cw.DrawString(text);
}
void flightplugin::crayons(CanvasWrapper cw, int x, int y)
{
	int marginLeft = 20;
	int marginTop = 20;
	int titleSpacing = 120;
	int nameSpacing = 130;
	int vecSpacing = 70;
	int quatSpacing = 120;
	int lineSpacing = 30;
	cw.SetPosition({ x, y });
	cw.SetColor(COLOR_PANEL);
	cw.FillBox({ 360, 350 });
	cw.SetColor(COLOR_TEXT);
	cw.SetColor(205, 155, 15, 255);
	this->drawStringAt(cw, "Instrument Panel", x + titleSpacing, y + marginTop);
	int currentLine = marginTop + 20;
	cw.SetColor(255, 255, 255, 255);
	currentLine += lineSpacing;
	this->drawStringAt(cw, "Liftmag", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(*liftmag, 5), x + marginLeft + nameSpacing, y + currentLine);
	currentLine += lineSpacing;
	this->drawStringAt(cw, "Dragmag", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(*dragmag, 5), x + marginLeft + nameSpacing, y + currentLine);
}