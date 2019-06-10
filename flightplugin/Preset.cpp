#include "Preset.h"

Preset Preset::FillPreset(int preset)
{
	switch (preset)
	{
	case 0: return { 1.f, 1.f, 0.f, 1.f,1.f,1.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,1.f }; // Default
	case 1: return { 2.067, .577, .035, .978, 1.218, .481, .065, .065, .321, .321, .304, .240, 2.468, 1.f }; // Digby
	}
	return { 1.f, 1.f, 0.f, 1.f,1.f,1.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,1.f };
}
