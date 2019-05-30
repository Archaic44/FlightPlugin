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
	enabled = make_shared<bool>(false);
	rho = make_shared<float>(0.f);
	boost = make_shared<float>(1.f);
	max_speed = make_shared<float>(1.f);
	length = make_shared<float>(1.f);
	width = make_shared<float>(1.f);
	height = make_shared<float>(1.f);
	x_scalar = make_shared<float>(0.f);
	y_scalar = make_shared<float>(0.f);
	z_scalar = make_shared<float>(0.f);
	pitch_scalar = make_shared<float>(0.f);
	roll_scalar = make_shared<float>(0.f);
	yaw_scalar = make_shared<float>(0.f);
	up_scalar = make_shared<float>(0.f);
	forceMode = make_shared<int>(0);

	gameWrapper->HookEvent("Function TAGame.GameEvent_Tutorial_TA.OnInit", bind(&flightplugin::OnFreeplayLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Tutorial_TA.Destroyed", bind(&flightplugin::OnFreeplayDestroy, this, std::placeholders::_1));

	cvarManager->registerCvar("flight_enabled", "0", "Enables/disable flight lift functionality", true, true, 0.f, true, 1.f)
		.addOnValueChanged(std::bind(&flightplugin::OnEnabledChanged, this, std::placeholders::_1, std::placeholders::_2));
	cvarManager->getCvar("flight_enabled").bindTo(enabled);
	cvarManager->registerCvar("air_density", "0", "Air Density", true, true, 0.f, true, 1.f, true).bindTo(rho);
	cvarManager->registerCvar("car_length", "1", "Car Length", true, true, 0.f, true, 10.f, true).bindTo(length);
	cvarManager->registerCvar("car_width", "1", "Car Width", true, true, 0.f, true, 10.f, true).bindTo(width);
	cvarManager->registerCvar("car_height", "1", "Car Height", true, true, 0.f, true, 10.f, true).bindTo(height);
	cvarManager->registerCvar("drag_x", "0", "Scales Drag X", true, true, 0.f, true, 10.f, true).bindTo(x_scalar);
	cvarManager->registerCvar("drag_y", "0", "Scales Drag Y", true, true, 0.f, true, 10.f, true).bindTo(y_scalar);
	cvarManager->registerCvar("drag_z", "0", "Scales Drag Z", true, true, 0.f, true, 10.f, true).bindTo(z_scalar);
	cvarManager->registerCvar("stabilize_pitch", "0", "Scales Pitch Stabilization", true, true, 0.f, true, 10.f, true).bindTo(pitch_scalar);
	cvarManager->registerCvar("stabilize_roll", "0", "Scales Roll Stabilization", true, true, 0.f, true, 10.f, true).bindTo(roll_scalar);
	cvarManager->registerCvar("stabilize_yaw", "0", "Scales Yaw Stabilization", true, true, 0.f, true, 10.f, true).bindTo(yaw_scalar);
	cvarManager->registerCvar("lift", "0", "Scales Up/Down Lift", true, true, 0.f, true, 10.f, true).bindTo(up_scalar);
	cvarManager->registerCvar("boost_power", "1", "Boost Power Multiplier", true, true, 0.f, true, 10.f).bindTo(boost);
	cvarManager->registerCvar("max_speed", "1", "Terminal Velocity Multiplier", true, true, 0.f, true, 10.f).bindTo(max_speed);

	logger.cvarManager = this->cvarManager;
	cmdManager.cvarManager = this->cvarManager;
	cmdManager.gameWrapper = this->gameWrapper;

	cmdManager.addCommands();

	painter.gameWrapper = this->gameWrapper;
	painter.cvarManager = this->cvarManager;
	painter.shared = this;

	painter.initDrawables();
}
void flightplugin::onUnload()
{
}
void flightplugin::OnFreeplayLoad(std::string eventName)
{
	if (*enabled)
		gameWrapper->HookEventWithCaller<CarWrapper>("Function TAGame.Car_TA.SetVehicleInput",
			bind(&flightplugin::OnSetInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}
void flightplugin::OnFreeplayDestroy(std::string eventName)
{
	gameWrapper->UnhookEvent("Function TAGame.RBActor_TA.PreAsyncTick");
}
void flightplugin::OnEnabledChanged(std::string oldValue, CVarWrapper cvar)
{
	if (cvar.getBoolValue() && gameWrapper->IsInFreeplay())
	{
		gameWrapper->HookEventWithCaller<CarWrapper>("Function TAGame.Car_TA.SetVehicleInput",
			bind(&flightplugin::OnSetInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
	else
	{
		cvarManager->log("Flight disabled");
		gameWrapper->UnhookEvent("Function TAGame.RBActor_TA.PreAsyncTick");
	}
}
Vector flightplugin::reflect_v1_on_v2(Vector incident, Vector n_unit)
{ // taken from: https://en.wikipedia.org/wiki/Specular_reflection
	float norm_dot = Vector::dot(incident, n_unit);
	if (norm_dot < 0) // Change the surface normal direction as air flux is hitting bottom/left/back face of car
	{
		n_unit = n_unit * -1;
		norm_dot *= -1.0;
	}
	norm_dot *= 2.0;
	Vector change = n_unit * norm_dot;
	Vector r_unit = change - incident;
	return r_unit; // Return unit vector of direction air bounces off car
}
void flightplugin::OnSetInput(CarWrapper cw, void * params, string funcName)
{
	if (gameWrapper->IsInFreeplay() && *enabled)
	{
		/* Definitions */
		auto car = cw;
		RBState rbstate = car.GetRBState();
		Vector loc = rbstate.Location;
		Vector lin = rbstate.LinearVelocity; // Car velocity with respect to world
		Vector v_unit = lin.norm();
		Vector ang = rbstate.AngularVelocity;
		float speed = lin.magnitude();
		Quat quat = rbstate.Quaternion;
		// The following vectors describe the basis of the car (the 3 sides of the car)
		Vector right = quatToRight(quat);
		Vector up = quatToUp(quat);
		Vector fwd = quatToFwd(quat);

		/* Begin Induced Drag Calculation*/
		// Momentum vectors for air bouncing off car
		Vector deflect_right = reflect_v1_on_v2(lin, right); // Simulate air bouncing off car left/right
		Vector deflect_up = reflect_v1_on_v2(lin, up); // Simulate air bouncing off car roof/bottom
		Vector deflect_fwd = reflect_v1_on_v2(lin, fwd); // Simulate air bouncing off car front/back

		// Resultant vector for car
		Vector res_right = (deflect_right + lin)/(-2.f);
		Vector res_up = (deflect_up + lin)/(-2.f);
		Vector res_fwd = (deflect_fwd + lin)/(-2.f);
		float coef = (*rho); // Apply reduction in speed

		Vector extent = car.GetLocalCollisionExtent();

		// Flux calculation
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
		float flux_r = Vector::dot(deflect_right.norm(), right) * door;
		float flux_u = Vector::dot(deflect_up.norm(), up) * roof;
		float flux_f = Vector::dot(deflect_fwd.norm(), fwd) * bumper;

		// Modify resultant vector by flux
		res_right = res_right * abs(flux_r *coef);
		res_up = res_up * abs(flux_u * coef);
		res_fwd = res_fwd * abs(flux_f * coef);
		Vector result = res_up + res_fwd + res_right;
		float scaleX = (*x_scalar);
		float scaleY = (*y_scalar);
		float scaleZ = (*z_scalar);
		result.X *= scaleX;
		result.Y *= scaleY;
		result.Z *= scaleZ;
		car.AddVelocity(result);

		/* Begin Stabilization Calculation */
		Vector axis_of_rotation = Vector::cross(fwd, lin);
		Vector rotation_scalars = Vector(*pitch_scalar, *roll_scalar, *yaw_scalar);
		Vector tau = axis_of_rotation.norm() * rotation_scalars * coef;
		car.SetAngularVelocity(tau,true);

		/* Begin Lift Calculation*/
		Vector up_lift = up * (*up_scalar) * Vector::dot(lin, fwd) *.001;
		car.AddVelocity(up_lift);

		car.SetMaxLinearSpeed(2300 * (*max_speed));
		car.SetMaxLinearSpeed2(2300 * (*max_speed));
		car.GetBoostComponent().SetBoostForce(178500*(*boost));
	}
}