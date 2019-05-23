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


public:
	std::shared_ptr<float> air_density, rho;
	std::shared_ptr<float> car_width, width;
	std::shared_ptr<float> car_height, height;
	std::shared_ptr<float> car_length, length;
	bool flight_enabled = false;
	virtual void onLoad();
	virtual void onUnLoad();
	Vector reflect_v1_on_v2(Vector v1, Vector v2);
	void OnSetInput();
	ofstream dump_file;
};
