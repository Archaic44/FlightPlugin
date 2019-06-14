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
		Preset tmp = Preset(2.067f, .577f, .035f, .978f, 1.218f, .481f, .065f, .065f, .321f, .321f, .304f, .240f, 2.468f, 50000.f); // Digby
		tmp.cvarManager = this->cvarManager;
		*this = tmp;
		break;
	}
	}
}

void Preset::SetPreset()
{
	cvarManager->executeCommand("fp_speed " + sp::to_string(pre[0], 5), false);
	cvarManager->executeCommand("fp_boost " + sp::to_string(pre[1], 5), false);
	cvarManager->executeCommand("fp_air_density " + sp::to_string(pre[2], 5), false);
	cvarManager->executeCommand("fp_length " + sp::to_string(pre[3], 5), false);
	cvarManager->executeCommand("fp_width " + sp::to_string(pre[4], 5), false);
	cvarManager->executeCommand("fp_height " + sp::to_string(pre[5], 5), false);
	cvarManager->executeCommand("fp_drag_x " + sp::to_string(pre[6], 5), false);
	cvarManager->executeCommand("fp_drag_y " + sp::to_string(pre[7], 5), false);
	cvarManager->executeCommand("fp_drag_z " + sp::to_string(pre[8], 5), false);
	cvarManager->executeCommand("fp_pitch " + sp::to_string(pre[9], 5), false);
	cvarManager->executeCommand("fp_roll " + sp::to_string(pre[10], 5), false);
	cvarManager->executeCommand("fp_yaw " + sp::to_string(pre[11], 5), false);
	cvarManager->executeCommand("fp_lift " + sp::to_string(pre[12], 5), false);
	cvarManager->executeCommand("fp_defaultThrottle " + sp::to_string(pre[13], 5), false);
}
