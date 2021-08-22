#pragma once

#include "main.hpp"

#include <string>
#include <unordered_map>

//#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(PluginConfig,

CONFIG_VALUE(IsEnabled, bool, "Mod Enabled", true);
CONFIG_VALUE(IsFormat, bool, "Format Mode", true);
    CONFIG_INIT_FUNCTION(
            CONFIG_INIT_VALUE(IsFormat);
            CONFIG_INIT_VALUE(IsEnabled);
    )
)