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

	liftp = make_shared<float>(0.0385);
	cvarManager->registerCvar("flight_lift", "0.0025", "Lift Power", true, true, 0, true, 1, true).bindTo(liftp);

	dragp = make_shared<float>(0.1);
	cvarManager->registerCvar("flight_drag", "0.1", "You flyin thru mud", true, true, 0, true, 1).bindTo(dragp);

	logger.cvarManager = this->cvarManager;
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
Vector flightplugin::reflect_v1_on_v2(Vector v1, Vector v2)
{
	Vector incident = v1.clone();
	Vector norm = v2.clone();
	Vector dot = Vector::dot(incident, norm);
	auto twodot = dot * 2;
	float x = twodot.X * norm.X + twodot.X * norm.Y + twodot.X * norm.Z;
	float y = twodot.Y * norm.X + twodot.Y * norm.Y + twodot.Y * norm.Z;
	float z = twodot.Z * norm.X + twodot.Z * norm.Y + twodot.Z * norm.Z;
	Vector reflect = Vector(x, y, z) - incident;
	return reflect*-1;
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
	Vector lin = rbstate.LinearVelocity; // Car velocity with respect to world
	if (lin.magnitude() > 100) { // Skip flight calculations if velocity is small
		Quat quat = rbstate.Quaternion;
		// The following vectors describe the basis of the car (the 3 sides of the car)
		Vector up = quatToUp(quat);  // Car's Up direction relative to world's xyz
		Vector right = quatToRight(quat); // Car's Right vector relative to world's xyz
		Vector fwd = quatToFwd(quat); // Car's forward vector relative to world's xyz
		
		// Linear Translation
		Vector air = lin*-1; // Still air being pushed into car
		Vector deflect_up = reflect_v1_on_v2(air, up); // Simulate air bouncing off car roof/bottom
		Vector deflect_right = reflect_v1_on_v2(air, right); // Simulate air bouncing off car left/right
		Vector deflect_fwd = reflect_v1_on_v2(air, fwd); // Simulate air bouncing off car front/back
		cvarManager->log(sp::vector_to_string(deflect_right));
		cvarManager->log(sp::vector_to_string(deflect_up));
		cvarManager->log(sp::vector_to_string(deflect_fwd));

		car.AddVelocity(deflect_up*.1);
		car.AddVelocity(deflect_right*.1);
		car.AddVelocity(deflect_fwd*.1);
	}
}