#pragma once
#pragma comment (lib, "bakkesmod.lib")
#include "bakkesmod\plugin\bakkesmodplugin.h"
#include "bakkesmod\wrappers\CVarManagerWrapper.h"
#include "bakkesmod\wrappers\GameWrapper.h"
#include "bakkesmod\wrappers\gamewrapper.h"
#include "bakkesmod\wrappers\wrapperstructs.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include "Logger.h"
#include "CommandsManager.h"
#include "Painter.h"

class flightplugin : public BakkesMod::Plugin::BakkesModPlugin
{
private:
	Logger logger;
	CommandsManager cmdManager;
	Painter painter;
	RBState oldCarRBState;
	std::chrono::time_point<std::chrono::steady_clock> last_time;
	std::chrono::time_point<std::chrono::steady_clock> start_time;
	std::shared_ptr<float> air_density, rho;
	std::shared_ptr<float> car_width, width;
	std::shared_ptr<float> car_height, height;
	std::shared_ptr<float> car_length, length;
	shared_ptr<bool> enabled;

public:
	virtual void onLoad();
	virtual void onUnload();

	void OnSetInput(CarWrapper cw, void* params, string funcName);
	void OnFreeplayLoad(std::string eventName);
	void OnFreeplayDestroy(std::string eventName);
	void OnEnabledChanged(std::string oldValue, CVarWrapper cvar);
	Vector reflect_v1_on_v2(Vector v1, Vector v2);

	ofstream dump_file;
};
