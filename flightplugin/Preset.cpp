#include "Preset.h"

Preset Preset::FillPreset(int preset)
{
	switch (preset)
	{
	case 0: return { 1.f, 1.f, 0.f, 1.f,1.f,1.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,12000.f }; // Default
	case 1: return { 2.067f, .577f, .035f, .978f, 1.218f, .481f, .065f, .065f, .321f, .321f, .304f, .240f, 2.468f, 50000.f }; // Digby
	}
	return { 1.f, 1.f, 0.f, 1.f,1.f,1.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,1.f };
}
