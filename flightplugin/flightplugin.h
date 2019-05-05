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

//START OF Logger.cpp
class Logger
{
public:
	void log(std::string s);
	void log(Vector v);
	void log(Rotator r);
	void log(float f);
	void log(bool b);
	void log(int i);
	void log(unsigned long l);
	std::shared_ptr<CVarManagerWrapper> cvarManager;
};
//START OF Painter.cpp
#define COLOR_TEXT 255c, 255c, 255c, 255c
#define COLOR_PANEL 64c, 64c, 64c, 192c
class CVarManagerWrapper;
inline constexpr char operator "" c(unsigned long long arg) noexcept
{
	return static_cast<char>(arg);
}
struct Color
{
	char r, g, b, a;
};
struct Vector2_ {
	int X;
	int Y;
	Vector2_(int x, int y) : X(x), Y(y) {}
	Vector2_ minus(Vector2_ other)
	{
		return{ X - other.X,  Y - other.Y };
	}
	inline Vector2_ operator+(Vector2_ v2) { return Vector2_(X + v2.X, Y + v2.Y); }
	inline Vector2_ operator*(float s) { return Vector2_(X * s, Y * s); }
	inline Vector2_ rotate(float theta) {
		float x = cos(theta) * X - sin(theta) * -Y;
		float y = sin(theta) * X + cos(theta) * -Y;
		return Vector2_(x, -y);
	}
};
class Painter
{
public:
	std::shared_ptr<GameWrapper> gameWrapper;
	std::shared_ptr<CVarManagerWrapper> cvarManager;
	void initDrawables();
private:
	void drawStringAt(CanvasWrapper cw, std::string text, int x, int y, Color col = { 255c, 255c, 255c, 255c });
	void drawStringAt(CanvasWrapper cw, std::string text, Vector2_ loc, Color col = { 255c, 255c, 255c, 255c });
	void drawLine(CanvasWrapper cw, Vector2_ x, Vector2_ y);
	void drawPanels(CanvasWrapper cw);
	void drawRBStatePanel(CanvasWrapper cw, std::string title, RBState rbstate, int x, int y, bool recording);
	void drawCarDerivedInfo(CanvasWrapper cw, CarWrapper car, int x, int y);
	void drawYawPlane(CanvasWrapper cw, CarWrapper car, int x, int y, float scale);
	float lift();
};
//START OF HelperFunctions.cpp
namespace sp
{
	template <typename T>
	std::string to_string_decimals(const T a_value, int n)
	{
		std::ostringstream out;
		out << std::fixed << std::setprecision(n) << a_value;
		return out.str();
	}
	template <typename T>
	std::string to_string_decimals_scientific(const T a_value, int n, int treshold)
	{
		std::ostringstream out;
		if (abs(a_value) > (float)pow(10, -treshold)) // for big enough values, just use normal notation
			out << std::fixed << std::setprecision(n) << a_value;
		else
			out << std::fixed << std::setprecision(n - treshold) << std::scientific << a_value;
		return out.str();
	}
	std::string vector_to_string(Vector v, int prec = 2);
	std::string rot_to_string(Rotator v, int prec = 8);
	std::string quat_to_string(Quat v, int prec = 10);
	std::string to_string(float f, int prec = 2);
	std::string to_string_scientific(float f, int prec = 11, int treshold = 4);
	bool vector_eq(Vector v1, Vector v2);
	bool quat_eq(Quat q1, Quat q2);
	bool rot_eq(Rotator r1, Rotator r2);
	Vector quatToFwd(Quat quat);
	Vector quatToRight(Quat quat);
	Vector quatToUp(Quat quat);
	Vector rotateVectorWithQuat(Vector v, Quat q);
}
//START OF CommandsManager.cpp
class CommandsManager
{
public:
	std::shared_ptr<CVarManagerWrapper> cvarManager;
	std::shared_ptr<GameWrapper> gameWrapper;
	Logger logger;
	void addCommands();
};
//START OF  flightplugin.cpp
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
