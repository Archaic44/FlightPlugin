#define LINMATH_H //Conflicts with linmath.h if we done declare this here

#include "flightplugin.h"
#include "bakkesmod\wrappers\ArrayWrapper.h"
#include "bakkesmod\wrappers\GameObject/CarWrapper.h"
#include "bakkesmod\wrappers\GameObject\CarComponent\WheelWrapper.h"
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
#include <filesystem>
#include "Preset.h"

BAKKESMOD_PLUGIN(flightplugin, "Flight plugin", "1.0.0", PLUGINTYPE_FREEPLAY)

using namespace sp;

void flightplugin::onLoad()
{
	enabled = make_shared<bool>(false);
	create = make_shared<bool>(false);
	fp_delete = make_shared<bool>(false);
	name = make_shared<std::string>("");
	rho = make_shared<float>(0.0f);
	boost = make_shared<float>(1.0f);
	max_speed = make_shared<float>(1.0f);
	length = make_shared<float>(1.0f);
	width = make_shared<float>(1.0f);
	height = make_shared<float>(1.0f);
	x_drag = make_shared<float>(0.f);
	y_drag = make_shared<float>(0.f);
	z_drag = make_shared<float>(0.f);
	pitch_scalar = make_shared<float>(0.0f);
	roll_scalar = make_shared<float>(0.0f);
	yaw_scalar = make_shared<float>(0.0f);
	lift = make_shared<float>(0.0f);
	forceMode = make_shared<int>(0);
	throttle = make_shared<float>(1.0f);
	preset_int = make_shared<int>(0);
	preset = std::make_shared<Preset>(Preset());

	logger.cvarManager = this->cvarManager;
	cmdManager.cvarManager = this->cvarManager;
	preset->cvarManager = this->cvarManager;
	painter.cvarManager = this->cvarManager;

	cmdManager.gameWrapper = this->gameWrapper;
	painter.gameWrapper = this->gameWrapper;
	painter.shared = this;

	cmdManager.addCommands();
	painter.initDrawables();
	cvarManager->registerCvar("fp_name_preset", "[Name of Preset]", "Name your custom flight plugin preset", true, false, 0, false, 1, false).bindTo(name);
	cvarManager->registerCvar("fp_air_density", "0", "Air Density", true, true, 0.f, true, 0.5f, true).bindTo(rho);
	cvarManager->registerCvar("fp_length", "1", "Car Length", true, true, 0.f, true, 10.f, true).bindTo(length);
	cvarManager->registerCvar("fp_width", "1", "Car Width", true, true, 0.f, true, 10.f, true).bindTo(width);
	cvarManager->registerCvar("fp_height", "1", "Car Height", true, true, 0.f, true, 10.f, true).bindTo(height);
	cvarManager->registerCvar("fp_drag_x", "0", "Scales Drag X", true, true, 0.f, true, 10.f, true).bindTo(x_drag);
	cvarManager->registerCvar("fp_drag_y", "0", "Scales Drag Y", true, true, 0.f, true, 10.f, true).bindTo(y_drag);
	cvarManager->registerCvar("fp_drag_z", "0", "Scales Drag Z", true, true, 0.f, true, 10.f, true).bindTo(z_drag);
	cvarManager->registerCvar("fp_pitch", "0", "Scales Pitch Stabilization", true, true, 0.f, true, 10.f, true).bindTo(pitch_scalar);
	cvarManager->registerCvar("fp_roll", "0", "Scales Roll Stabilization", true, true, 0.f, true, 10.f, true).bindTo(roll_scalar);
	cvarManager->registerCvar("fp_yaw", "0", "Scales Yaw Stabilization", true, true, 0.f, true, 10.f, true).bindTo(yaw_scalar);
	cvarManager->registerCvar("fp_lift", "0", "Scales Up/Down Lift", true, true, 0.f, true, 10.f, true).bindTo(lift);
	cvarManager->registerCvar("fp_boost", "1", "Boost Power Multiplier", true, true, 0.f, true, 10.f).bindTo(boost);
	cvarManager->registerCvar("fp_speed", "1", "Terminal Velocity Multiplier", true, true, 0.000499, true, 10.f).bindTo(max_speed);
	cvarManager->registerCvar("fp_throttle", "1", "Air Throttle Force Multiplier", true, true, 0.f, true, 10.f, false).bindTo(throttle);

	cvarManager->registerCvar("fp_enabled", "0", "Enables/disable flight lift functionality", true, true, 0.f, true, .5f, false)
		.addOnValueChanged(std::bind(&flightplugin::OnEnabledChanged, this, std::placeholders::_1, std::placeholders::_2));
	cvarManager->getCvar("fp_enabled").bindTo(enabled);
	cvarManager->registerCvar("fp_preset", "0", "Presets for slider values", true, true, 0, true, 2000)
		.addOnValueChanged(bind(&flightplugin::OnPresetChanged, this, std::placeholders::_1, std::placeholders::_2));
	cvarManager->getCvar("fp_preset").bindTo(preset_int);
	cvarManager->registerCvar("fp_create", "0", "Create your custom flight plugin preset", true, true, 0, true, 1, false)
		.addOnValueChanged(bind(&flightplugin::OnCreateChanged, this, std::placeholders::_1, std::placeholders::_2));
	cvarManager->getCvar("fp_create").bindTo(create);
	cvarManager->registerCvar("fp_delete", "0", "Delete your custom flight plugin preset", true, true, 0, true, 1, false)
		.addOnValueChanged(bind(&flightplugin::OnDeleteChanged, this, std::placeholders::_1, std::placeholders::_2));
	cvarManager->getCvar("fp_create").bindTo(fp_delete);

	gameWrapper->HookEvent("Function TAGame.Mutator_Freeplay_TA.Init", bind(&flightplugin::OnFreeplayLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.Destroyed", bind(&flightplugin::OnFreeplayDestroy, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.StartPlayTest", bind(&flightplugin::OnFreeplayLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.Destroyed", bind(&flightplugin::OnFreeplayDestroy, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function GameEvent_TA.Countdown.OnPlayerRestarted", bind(&flightplugin::OnResetShot, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function ProjectX.GFxDataStore_X.CreateObject", bind(&flightplugin::OnSpawn, this, std::placeholders::_1));
	cvarManager->executeCommand("cl_settings_refreshplugins",false);
}
inline string ArrayToString(ArrayHolder* array, int size)
{
	string returnstring = "";
	for (int temp = 0; temp < size; temp++)
	{
		returnstring += sp::to_string(array->array[temp], 3);
		returnstring += " ";
	}
	return returnstring;
}
inline bool FileExist(const std::string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}
void flightplugin::OnCreateChanged(std::string eventName, CVarWrapper cvar)
{
	if (!cvar.getBoolValue())
		return;
	// Get preset name
	if (CreateDirectory("./bakkesmod/flightplugin", NULL))
	{
		cvarManager->log("Created flightplugin directory.");
	}
	else if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		cvarManager->log("Directory flightplugin already exists.");
	}
	cvarManager->log("Attempting to create preset...");
	auto preset_name = *name;
	Preset tmp = Preset(*max_speed, *boost, *rho, *length, *width, *height, *x_drag, *y_drag, 
					*z_drag, *pitch_scalar, *roll_scalar, *yaw_scalar, *lift, *throttle);

	// Do stuff to .set file
	std::ifstream filein("./bakkesmod/plugins/settings/flightplugin.set");
	std::ofstream fileout("./bakkesmod/plugins/settings/tmp.set");
	if (!filein.is_open() || !fileout.is_open())
	{
		cvarManager->log("Error opening flightplugin.set files. Close flightplugin.set if open in an editor.");
		return;
	}
	else
	{
		string strTemp;
		string subStr = "&" + preset_name + "@" + preset_name;
		string defaultPreset = preset_name + "@";
		while (std::getline(filein, strTemp))
		{
			if (strTemp.find("fp_preset") != string::npos)
			{
				if (strTemp.find(subStr) != string::npos)
				{
					cvarManager->log("Preset name already exists! Overwriting slider values!");
				}
				else if (strTemp.find(preset_name) != string::npos)
				{
					cvarManager->log("Cannot overwrite base preset. Choose a different name.");
					return;
				}
				else
					strTemp += "&" + preset_name + "@" + preset_name;
			}
			fileout << strTemp << '\n';
		}
		filein.close();
		fileout.close();
		cvarManager->log("Wrote to tmp.set");
	}
	char mode[] = "0644";
	int i = strtol(mode, 0, 8);
	if (_chmod("./bakkesmod/plugins/settings/flightplugin.set", i))
		cvarManager->log("Unable to modify flightplugin.set permissions");
	else
		cvarManager->log("Updated flightplugin.set permissions.");

	if (remove("./bakkesmod/plugins/settings/flightplugin.set"))
		cvarManager->log("Error deleting flightplugin.set. Errno: " + string(strerror(errno)));
	else
		cvarManager->log("flightplugin.set successfully deleted");

	if (rename("./bakkesmod/plugins/settings/tmp.set", "./bakkesmod/plugins/settings/flightplugin.set"))
		cvarManager->log("Failed to rename tmp.set to flightplugin.set: " + string(strerror(errno)));
	else
		cvarManager->log("Preset " + preset_name + " added to flightplugin.set!");

	std::ofstream out("./bakkesmod/flightplugin/" + preset_name + ".txt");
	if (!out.is_open())
	{
		cvarManager->log("Error opening preset file. Close preset.txt if open in an editor.");
		return;
	}
	else
	{
		std::string preset_contents = ArrayToString(tmp.GetArray(), tmp.GetArraySize());
		// Writes preset vals to flightplugin/{preset_name}.txt
		out << preset_contents;
		out.close();
		cvarManager->log("Wrote to " + preset_name + ".txt");
	}


	cvarManager->executeCommand("cl_settings_refreshplugins", false);

}
void flightplugin::OnDeleteChanged(std::string eventName, CVarWrapper cvar)
{
	auto preset_name = *name;
	// Open flightplugin.set and remove that line
	std::ifstream filein("./bakkesmod/plugins/settings/flightplugin.set");
	std::ofstream fileout("./bakkesmod/plugins/settings/tmp.set");
	if (!filein.is_open() || !fileout.is_open())
	{
		cvarManager->log("Error opening flightplugin.set files. Close flightplugin.set if open in an editor.");
	}
	else
	{
		bool presetFound = false;
		bool isBase = false;
		string strTemp;
		string subStr = "&" + preset_name + "@" + preset_name;
		string defaultPreset = preset_name + "@";
		while (std::getline(filein, strTemp))
		{
			size_t pos = strTemp.find(subStr);
			if (pos != string::npos)
			{
				cvarManager->log("Removing " + preset_name + " from flightplugin.set!");
				presetFound = true;
				strTemp.erase(pos, subStr.length());
			}
			else if (strTemp.find(defaultPreset) != string::npos)
			{
				cvarManager->log("Cannot remove base preset!");
				isBase = true;
			}
			fileout << strTemp << '\n';
		}
		filein.close();
		fileout.close();
		if (!presetFound)
		{
			if (remove("./bakkesmod/plugins/settings/tmp.set"))
				cvarManager->log("Error removing tmp.set");
			else
				cvarManager->log("Removed tmp.set");
			if (isBase)
				return;
			cvarManager->log("Preset not found. Check your preset name!");

			return;
		}
		cvarManager->log("Wrote to tmp.set...");
	}
	if (remove("./bakkesmod/plugins/settings/flightplugin.set"))
		cvarManager->log("Error deleting flightplugin.set. Errno: " + string(strerror(errno)));
	else
		cvarManager->log("flightplugin.set successfully deleted");

	if (rename("./bakkesmod/plugins/settings/tmp.set", "./bakkesmod/plugins/settings/flightplugin.set"))
		cvarManager->log("Failed to rename tmp.set to flightplugin.set: " + string(strerror(errno)));
	else
		cvarManager->log("Moved tmp.set into flightplugin.set! Preset " + preset_name + " removed!");
	string file_name = "./bakkesmod/flightplugin/" + preset_name + ".txt";
	if (remove(file_name.c_str()))
		cvarManager->log("Failed to remove file: flightplugin/" + preset_name + ".txt");
	cvarManager->executeCommand("cl_settings_refreshplugins", false);
}
void flightplugin::RemovePhysics(CarWrapper cw)
{
	if(cw.IsNull())
	{
		return;
	}
	auto car = cw;
	car.SetMaxLinearSpeed(2300);
	car.SetMaxLinearSpeed2(2300);
	car.GetBoostComponent().SetBoostForce(178500);
	AirControlComponentWrapper acc = car.GetAirControlComponent();
	acc.SetThrottleForce(12000);
	acc.SetAirTorque({ 130, 95, 400 });
}
void flightplugin::onUnload()
{
	cvarManager->log("Debug5 statement");

	if (gameWrapper->IsInGame())
	this->RemovePhysics(gameWrapper->GetLocalCar());
}
void flightplugin::OnFreeplayLoad(std::string eventName)
{
	if (*enabled)
	{
		gameWrapper->HookEventWithCaller<CarWrapper>("Function TAGame.Car_TA.SetVehicleInput",
			bind(&flightplugin::OnSetInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
	else 
	{
		gameWrapper->UnhookEvent("Function TAGame.Car_TA.SetVehicleInput");
	}
}
void flightplugin::OnResetShot(std::string eventName)
{
	if (*enabled)
	{
		wasEnabled = true;
	}
	*enabled = false;
}
void flightplugin::OnSpawn(std::string eventName)
{
	if (wasEnabled)
	{
		wasEnabled = false;
		*enabled = true;
	}
}
void flightplugin::OnFreeplayDestroy(std::string eventName)
{
	gameWrapper->UnhookEvent("Function TAGame.Car_TA.SetVehicleInput");
}
inline bool isInteger(const std::string& s)
{
	if (s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

	char* p;
	strtol(s.c_str(), &p, 10);

	return (*p == 0);
}
void flightplugin::OnPresetChanged(std::string oldvalue, CVarWrapper cvar)
{
	auto val = cvar.getStringValue();
	//string tmp = cvar.
	if (isInteger(val))
	{
		preset->FillPreset(cvar.getIntValue());
		preset->SetPreset();
	}
	else
	{
		std::ifstream filein("./bakkesmod/flightplugin/" + val + ".txt");
		float element;
		ArrayHolder tmp;
		if (filein.is_open())
		{
			int i = 0;
			while (filein >> element)
			{
				tmp.array[i++] = element;
			}
			filein.close();
			preset->SetArray(&tmp);
			preset->SetPreset();
		}
	}
}
void flightplugin::OnEnabledChanged(std::string oldValue, CVarWrapper cvar)
{

	auto cvarName = cvar.getCVarName();
	bool inSafe = gameWrapper->IsInFreeplay() | gameWrapper->IsInCustomTraining() | gameWrapper->IsInGame();

	if (cvar.getBoolValue() && inSafe)
	{
		gameWrapper->HookEventWithCaller<CarWrapper>("Function TAGame.Car_TA.SetVehicleInput",
			bind(&flightplugin::OnSetInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
	else if (!cvar.getBoolValue() && inSafe)
	{
		gameWrapper->UnhookEvent("Function TAGame.Car_TA.SetVehicleInput");
		this->RemovePhysics(gameWrapper->GetLocalCar());
	}
	else
	{
		gameWrapper->UnhookEvent("Function TAGame.Car_TA.SetVehicleInput");
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
	if (gameWrapper->IsInFreeplay() && *enabled && !cw.IsNull() && !cw.GetBoostComponent().IsNull())
	{
		/* Definitions */
		auto car = cw;
		RBState rbstate = car.GetRBState();
		Vector loc = rbstate.Location;
		Vector lin = rbstate.LinearVelocity; // Car velocity with respect to world
		Vector v_unit = lin.clone();
		v_unit.normalize();
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
		Vector d_r = deflect_right.clone();
		Vector d_u = deflect_up.clone();
		Vector d_f = deflect_fwd.clone();
		d_r.normalize(), d_u.normalize(), d_f.normalize();

		// Resultant vector for car
		Vector res_right = (deflect_right + lin) / (-2.f);
		Vector res_up = (deflect_up + lin) / (-2.f);
		Vector res_fwd = (deflect_fwd + lin) / (-2.f);
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
		float flux_r = Vector::dot(d_r, right) * door;
		float flux_u = Vector::dot(d_u, up) * roof;
		float flux_f = Vector::dot(d_f, fwd) * bumper;

		// Scale resulting drag vector by flux and sliders
		res_right = res_right * abs(flux_r * coef);
		res_up = res_up * abs(flux_u * coef);
		res_fwd = res_fwd * abs(flux_f * coef);
		Vector result = res_up + res_fwd + res_right;
		result.X *= (*x_drag);
		result.Y *= (*y_drag);
		result.Z *= (*z_drag);
		car.AddVelocity(result);

		/* Begin Stabilization Calculation */
		Vector axis_of_rotation = Vector::cross(fwd, lin);
		Vector a_o_r = axis_of_rotation.clone();
		a_o_r.normalize();
		Vector rotation_scalars = Vector(*pitch_scalar, *roll_scalar, *yaw_scalar);
		Vector tau = a_o_r * rotation_scalars * coef;
		car.SetAngularVelocity(tau, true);

		/* Begin Lift Calculation*/
		Vector wing_lift = up * (*lift) * Vector::dot(lin, fwd) * .2;
		Vector front_pressure = fwd * (*lift) * Vector::dot(lin,fwd) * -.05;
		Vector side_pressure = right * (*lift) * Vector::dot(lin, right) * -.25;
		Vector top_pressure = up * (*lift) * Vector::dot(lin, up) * -.25;
		Vector lift = (wing_lift + front_pressure + side_pressure + top_pressure) * coef;
		car.AddVelocity(lift);

		car.SetMaxLinearSpeed(2300 * (*max_speed)); // 2300 is the OG max speed
		car.SetMaxLinearSpeed2(2300 * (*max_speed));
		car.GetBoostComponent().SetBoostForce(178500 * (*boost)); //178500 is the OG BoostSpeed


		/* oh neat */
		Rotator defaultTorque = { 130, 95, 400 };

		AirControlComponentWrapper acc = car.GetAirControlComponent();
		acc.SetThrottleForce(12000 * (*throttle));
		float speedTorqueRatio = 1300 / speed;
		Rotator newTorque = (defaultTorque * speedTorqueRatio);
		acc.SetAirTorque(newTorque);

		/*Auto Stickywheels*/
		bool grounded = car.IsOnGround();
		if (grounded && speed >= 1300)
		{
			car.SetStickyForce({ 0.f,0.f });
		}
		else
		{
			car.SetStickyForce({ 0.5,1.5 }); // OG values are  .5, 1.5 -- Ground/Wall
			acc.SetAirTorque(defaultTorque);
		}
	}
}