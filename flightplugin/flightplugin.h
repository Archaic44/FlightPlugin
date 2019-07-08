#pragma once
#pragma comment (lib, "bakkesmod.lib")
#include "bakkesmod\plugin\bakkesmodplugin.h"
#include "bakkesmod\wrappers\CVarManagerWrapper.h"
#include "bakkesmod\wrappers\GameWrapper.h"
#include "bakkesmod\wrappers\gamewrapper.h"
#include "bakkesmod\wrappers\wrapperstructs.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <io.h>
#include "Logger.h"
#include "CommandsManager.h"
#include "Painter.h"
#include "Preset.h"

class flightplugin : public BakkesMod::Plugin::BakkesModPlugin
{
private:
	Logger logger;
	CommandsManager cmdManager;
	Painter painter;
	RBState oldCarRBState;
	bool wasEnabled = false;
	std::chrono::time_point<std::chrono::steady_clock> last_time;
	std::chrono::time_point<std::chrono::steady_clock> start_time;
	std::shared_ptr<Preset> preset;

public:
	virtual void onLoad();
	virtual void onUnload();

	std::shared_ptr<float> rho;
	std::shared_ptr<float> boost, max_speed;
	std::shared_ptr<float> length, width, height;
	std::shared_ptr<float> x_drag, y_drag, z_drag;
	std::shared_ptr<float> pitch_scalar, roll_scalar, yaw_scalar;
	std::shared_ptr<float> lift;
	std::shared_ptr<int> forceMode;
	std::shared_ptr<int> preset_int;
	std::shared_ptr<bool> enabled, create, fp_delete;
	std::shared_ptr<std::string> name;
	std::shared_ptr<float> throttle;
	std::shared_ptr<float> utq;

	void OnSetInput(CarWrapper cw, void* params, string funcName);
	void OnFreeplayLoad(std::string eventName);
	void OnFreeplayDestroy(std::string eventName);
	void OnEnabledChanged(std::string oldValue, CVarWrapper cvar);
	void OnPresetChanged(std::string oldvalue, CVarWrapper cvar);
	void OnResetShot(std::string eventName);
	void OnSpawn(std::string eventName);
	void OnCreateChanged(std::string eventName, CVarWrapper cvar);
	void OnDeleteChanged(std::string eventName, CVarWrapper cvar);
	void RemovePhysics(CarWrapper cw);
	Vector reflect_v1_on_v2(Vector v1, Vector v2);
	ofstream dump_file;
};
