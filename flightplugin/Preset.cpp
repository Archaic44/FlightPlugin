#include "Preset.h"

void Preset::FillPreset(int preset)
{
	switch (preset){
	case 0:
	{
		Preset tmp = Preset(); // Default
		tmp.cvarManager = this->cvarManager;
		*this = tmp;
		break;
	}
	case 1:
	{
		Preset tmp = Preset(2.067f, .577f, .035f, .978f, 1.218f, .481f, .065f, .065f, .321f, .321f, .304f, .240f, .171f, 6.724f); // Digby
		tmp.cvarManager = this->cvarManager;
		*this = tmp;
		break;
	}
	case 2:
	{
		Preset tmp = Preset(10.000f, 2.003f, 0.020f, 4.177f, 1.040f, 0.202f, 2.609f, 2.640f, 0.481f, 3.944f, 4.348f, 4.425f, 0.233f, 10.000); // Blackbird
		tmp.cvarManager = this->cvarManager;
		*this = tmp;
		break;
	}
	case 3:
	{
		Preset tmp = Preset(2.081f, 0.000f, 0.206f, 0.978f, 1.218f, 0.481f, 1.056f, 1.071f, 0.062f, 0.000f, 0.000f, 0.326f, 3.012f, 10.000); // Hot Air Balloon
		tmp.cvarManager = this->cvarManager;
		*this = tmp;
		break;
	}
	case 4:
	{
		Preset tmp = Preset(0.714f, 0.000f, 0.030f, 10.000f, 10.000f, 0.001f, 0.512f, 0.543f, 0.575f, 0.404f, 0.000f, 0.000f, 0.839f, 4.565f); // Magic Carpet
		tmp.cvarManager = this->cvarManager;
		*this = tmp;
		break;
	}
	case 5:
	{
		Preset tmp = Preset(0.000f, 0.000f, 0.500f, 0.000f, 0.000f, 0.000f, 10.000f, 10.000f, 10.000f, 0.000f, 0.000f, 10.000f, 0.000f, 0.000); // jsonV's Merry-Go-Round
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
void Preset::SetPreset(){
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
ArrayHolder* Preset::GetArray(){
	return &preset;
}
void Preset::SetArray(ArrayHolder* val){
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
int Preset::GetArraySize(){
	return 14;
}
