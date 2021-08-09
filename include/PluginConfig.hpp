#pragma once

#include "main.hpp"

#include <string>
#include <unordered_map>

//#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(PluginConfig,

CONFIG_VALUE(IsEnabled, bool, "Mod Enabled", true);
CONFIG_VALUE(MaxSpeed, float, "Max Scroll Speed", 600);
CONFIG_VALUE(Accel, float, "Scroll Acceleration", 1.5);
CONFIG_VALUE(IsLinear, bool, "Scrolling is Linear", true);
    CONFIG_INIT_FUNCTION(
            CONFIG_INIT_VALUE(MaxSpeed);
            CONFIG_INIT_VALUE(Accel);
            CONFIG_INIT_VALUE(IsLinear);
            CONFIG_INIT_VALUE(IsEnabled);
    )
)