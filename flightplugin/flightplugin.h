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
#include "Arrow.h"
#include "CarManager.h"

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
	std::shared_ptr<float> flight_drag, dragp;
	std::shared_ptr<float> flight_lift, liftp;
	bool flight_enabled = false;
	virtual void onLoad();
	virtual void onUnLoad();
	void OnSetInput();
	ofstream dump_file;
};
