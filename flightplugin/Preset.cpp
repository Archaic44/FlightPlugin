#include "Preset.h"

void Preset::FillPreset(int preset)
{
	switch (preset)
	{
	case 0:
	{
		Preset tmp = Preset(); // Default
		tmp.cvarManager = this->cvarManager;
		*this = tmp;
		break;
	}
	case 1:
	{
		Preset tmp = Preset(2.067f, .577f, .035f, .978f, 1.218f, .481f, .065f, .065f, .321f, .321f, .304f, .240f, 2.468f, 3.5f); // Digby
		tmp.cvarManager = this->cvarManager;
		*this = tmp;
		break;
	}
	default:
	{
		Preset tmp = Preset(); // Default
		tmp.cvarManager = this->cvarManager;
		*this = tmp;
		break;
	}
	}
}

void Preset::SetPreset()
{
	cvarManager->executeCommand("fp_speed " + sp::to_string(preset.array[0], 5), false);
	cvarManager->executeCommand("fp_boost " + sp::to_string(preset.array[1], 5), false);
	cvarManager->executeCommand("fp_air_density " + sp::to_string(preset.array[2], 5), false);
	cvarManager->executeCommand("fp_length " + sp::to_string(preset.array[3], 5), false);
	cvarManager->executeCommand("fp_width " + sp::to_string(preset.array[4], 5), false);
	cvarManager->executeCommand("fp_height " + sp::to_string(preset.array[5], 5), false);
	cvarManager->executeCommand("fp_drag_x " + sp::to_string(preset.array[6], 5), false);
	cvarManager->executeCommand("fp_drag_y " + sp::to_string(preset.array[7], 5), false);
	cvarManager->executeCommand("fp_drag_z " + sp::to_string(preset.array[8], 5), false);
	cvarManager->executeCommand("fp_pitch " + sp::to_string(preset.array[9], 5), false);
	cvarManager->executeCommand("fp_roll " + sp::to_string(preset.array[10], 5), false);
	cvarManager->executeCommand("fp_yaw " + sp::to_string(preset.array[11], 5), false);
	cvarManager->executeCommand("fp_lift " + sp::to_string(preset.array[12], 5), false);
	cvarManager->executeCommand("fp_throttle " + sp::to_string(preset.array[13], 5), false);
}

ArrayHolder* Preset::GetArray()
{
	return &preset;
}
void Preset::SetArray(ArrayHolder* val)
{
	preset.array[0] = val->array[0];
	preset.array[1] = val->array[1];
	preset.array[2] = val->array[2];
	preset.array[3] = val->array[3];
	preset.array[4] = val->array[4];
	preset.array[5] = val->array[5];
	preset.array[6] = val->array[6];
	preset.array[7] = val->array[7];
	preset.array[8] = val->array[8];
	preset.array[9] = val->array[9];
	preset.array[10] = val->array[10];
	preset.array[11] = val->array[11];
	preset.array[12] = val->array[12];
	preset.array[13] = val->array[13];
}

int Preset::GetArraySize()
{
	return 14;
}
