#pragma once


class Preset
{
	float pre[14];
public:
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
	Preset FillPreset(int preset);
};