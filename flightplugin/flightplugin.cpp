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
	cvarManager->registerCvar("flight_lift", "4", "Lift Power", true, true, 0, true, 10, false).bindTo(liftmag);

	dragmag = make_shared<float>();
	cvarManager->registerCvar("flight_drag", "1", "You flyin thru mud", true, true, 0, true, 10, false).bindTo(dragmag);

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
		cvarManager->log("Time to fly!");
		}, "Starts the Flight mode.", PERMISSION_FREEPLAY);
}

void flightplugin::onUnLoad()
{
	flight_enabled = false;
	return;
}
void flightplugin::OnSetInput()
{
	if (flight_enabled == false) //stops the plugin if flight_enabled is false
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
	Vector drag = (rbstate.LinearVelocity * (*dragmag * (-1.0f)));
	car.AddForce(drag, 1);
}