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
	void drawStringAt(CanvasWrapper cw, std::string text, int x, int y, Color col = { 255c, 255c, 255c, 255c });
	void drawStringAt(CanvasWrapper cw, std::string text, Vector2_ loc, Color col = { 255c, 255c, 255c, 255c });
	void drawLine(CanvasWrapper cw, Vector2_ x, Vector2_ y);
	Logger logger;
	CommandsManager cmdManager;
	Painter painter;
	RBState oldCarRBState;
	std::chrono::time_point<std::chrono::steady_clock> last_time;
	std::chrono::time_point<std::chrono::steady_clock> start_time;

public:
	std::shared_ptr<float> flight_drag, dragmag;
	std::shared_ptr<float> flight_lift, liftmag;
	bool flight_enabled = false;
	virtual void onLoad();
	virtual void onUnLoad();
	void OnSetInput();
	ofstream dump_file;
	void crayons(CanvasWrapper cw, int x, int y);
};
