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
	rho = make_shared<float>(.003);
	cvarManager->registerCvar("air_density", ".003", "Air Density", true, true, 0.0, true, 10.0, true).bindTo(rho);

	width = make_shared<float>(1.0);
	cvarManager->registerCvar("car_width", "1.0", "Car Width", true, true, 0.0, true, 10.0, true).bindTo(width);

	length = make_shared<float>(1.0);
	cvarManager->registerCvar("car_length", "1.0", "Car Length", true, true, 0.0, true, 10.0, true).bindTo(length);

	height = make_shared<float>(1.0);
	cvarManager->registerCvar("car_height", "1.0", "Car Height", true, true, 0.0, true, 10.0, true).bindTo(height);


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
Vector flightplugin::reflect_v1_on_v2(Vector incident, Vector n_unit)
{ // taken from: https://en.wikipedia.org/wiki/Specular_reflection
	float norm_dot = Vector::dot(incident, n_unit);
	if (norm_dot < 0) // Change the surface normal direction as air flux is hitting bottom/left/back face of car
	{
		n_unit = n_unit*-1;
		norm_dot *= -1.0;
	}
	norm_dot *= 2.0;
	Vector change = n_unit * norm_dot;
	Vector r_unit = change - incident;
	cvarManager->log("Norm_unit: " + sp::vector_to_string(n_unit, 5) + " " + sp::to_string(norm_dot));
	return r_unit; // Return unit vector of direction air bounces off car
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
	float speed = lin.magnitude();
	Quat quat = rbstate.Quaternion;
	// The following vectors describe the basis of the car (the 3 sides of the car)
	Vector right = quatToRight(quat); 
	Vector up = quatToUp(quat);  
	Vector fwd = quatToFwd(quat);

	// Linear Translation
	Vector deflect_right = reflect_v1_on_v2(lin, right); // Simulate air bouncing off car left/right
	Vector deflect_up = reflect_v1_on_v2(lin, up); // Simulate air bouncing off car roof/bottom
	Vector deflect_fwd = reflect_v1_on_v2(lin, fwd); // Simulate air bouncing off car front/back
	cvarManager->log("Right: " + sp::vector_to_string(deflect_right));
	cvarManager->log("Up: " + sp::vector_to_string(deflect_up));
	cvarManager->log("Fwd: " + sp::vector_to_string(deflect_fwd));

	float coef = (*rho); // Apply reduction in speed
	cvarManager->log(sp::to_string(speed,4));
	cvarManager->log(sp::to_string(coef,5));
	Vector extent = car.GetLocalCollisionExtent();
	float w = extent.Y * (*width);
	float h = extent.Z * (*height);
	float l = extent.X * (*length);
	float roof_area = w * l;
	float door_area = l * h;
	float bumper_area = w * h;
	float total_area = roof_area + door_area + bumper_area;
	float roof = roof_area / total_area;
	float door = door_area / total_area;
	float bumper = bumper_area / total_area;
	float flux_r = -1 * abs(Vector::dot(deflect_right.norm(), right) * door * coef);
	float flux_u = -1 * abs(Vector::dot(deflect_up.norm(), up) * roof * coef);
	float flux_f = -1 * abs(Vector::dot(deflect_fwd.norm(), fwd) * bumper * coef);
	cvarManager->log(sp::to_string(flux_r,5) + "   " + sp::to_string(flux_u,5) + "   " + sp::to_string(flux_f,5));
	deflect_right = deflect_right * flux_r;
	deflect_up = deflect_up * flux_u;
	deflect_fwd = deflect_fwd * flux_f;
	Vector deflect = deflect_fwd + deflect_right + deflect_up;
	cvarManager->log("Rscaled: "+sp::vector_to_string(deflect_right,5));
	cvarManager->log("Uscaled: "+sp::vector_to_string(deflect_up,5));
	cvarManager->log("Fscaled: "+sp::vector_to_string(deflect_fwd,5));
	cvarManager->log("DEF: "+sp::vector_to_string(deflect,5));
	car.AddVelocity(deflect);
}