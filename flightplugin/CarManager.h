#pragma once
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include <vector>
#include <string>

class CarManager : public BakkesMod::Plugin::BakkesModPlugin
{
public:
	CarManager();
	static const std::string getHelpText();
	static const std::vector<Vector> getArrowPoints(CARBODY car_type, GameWrapper& gameWrapper);
	~CarManager();
};