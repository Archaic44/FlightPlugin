#pragma once
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "HelperFunctions.h"
using namespace sp;
struct ArrayHolder
{
	float array[14];
};
class Preset
{
	ArrayHolder preset;
public:
	Preset() // Default Preset Constructor
	{
		preset.array[0] = 1.f;
		preset.array[1] = 1.f;
		preset.array[2] = 0.f;
		preset.array[3] = 1.f;
		preset.array[4] = 1.f;
		preset.array[5] = 1.f;
		preset.array[6] = 0.f;
		preset.array[7] = 0.f;
		preset.array[8] = 0.f;
		preset.array[9] = 0.f;
		preset.array[10] = 0.f;
		preset.array[11] = 0.f;
		preset.array[12] = 0.f;
		preset.array[13] = 1.f;
	}
	Preset(float speed, float boost, float density, float carL, float carW, float carH,
		float xDrag, float yDrag, float zDrag, float pitch, float roll, float yaw,
		float lift, float throttle) 
	{
		preset.array[0] = speed;
		preset.array[1] = boost;
		preset.array[2] = density;
		preset.array[3] = carL;
		preset.array[4] = carW;
		preset.array[5] = carH;
		preset.array[6] = xDrag;
		preset.array[7] = yDrag;
		preset.array[8] = zDrag;
		preset.array[9] = pitch;
		preset.array[10] = roll;
		preset.array[11] = yaw;
		preset.array[12] = lift;
		preset.array[13] = throttle;
	}
	float operator[] (int i)
	{
		return preset.array[i];
	}
	const float operator[](int i) const
	{
		return preset.array[i];
	}
	std::shared_ptr<CVarManagerWrapper> cvarManager;
	void FillPreset(int preset);
	void SetPreset();
	ArrayHolder* GetArray();
	void SetArray(ArrayHolder* val);
	int GetArraySize();
};