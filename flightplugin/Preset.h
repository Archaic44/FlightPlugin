#pragma once
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "HelperFunctions.h"
using namespace sp;

class Preset
{
	float pre[14];
public:
	Preset() // Default Preset Constructor
	{
		pre[0] = 1.f;
		pre[1] = 1.f;
		pre[2] = 0.f;
		pre[3] = 1.f;
		pre[4] = 1.f;
		pre[5] = 1.f;
		pre[6] = 0.f;
		pre[7] = 0.f;
		pre[8] = 0.f;
		pre[9] = 0.f;
		pre[10] = 0.f;
		pre[11] = 0.f;
		pre[12] = 0.f;
		pre[13] = 12000.f;
	}
	Preset(float speed, float boost, float density, float carL, float carW, float carH,
		float xDrag, float yDrag, float zDrag, float pitch, float roll, float yaw,
		float lift, float throttle) 
	{
		pre[0] = speed;
		pre[1] = boost;
		pre[2] = density;
		pre[3] = carL;
		pre[4] = carW;
		pre[5] = carH;
		pre[6] = xDrag;
		pre[7] = yDrag;
		pre[8] = zDrag;
		pre[9] = pitch;
		pre[10] = roll;
		pre[11] = yaw;
		pre[12] = lift;
		pre[13] = throttle;
	}
	float operator[] (int i)
	{
		return pre[i];
	}
	const float operator[](int i) const
	{
		return pre[i];
	}
	std::shared_ptr<CVarManagerWrapper> cvarManager;
	void FillPreset(int preset);
	void SetPreset();
};