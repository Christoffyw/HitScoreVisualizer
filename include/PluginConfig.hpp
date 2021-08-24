#pragma once

#include "main.hpp"

#include "UnityEngine/Color.hpp"

#include <string>
#include <unordered_map>

//#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(PluginConfig,

//General Mod Config
CONFIG_VALUE(IsEnabled, bool, "Mod Enabled", true);

//Judgment 1
CONFIG_VALUE(J1Threshold, int, "Judgment 1 Threshold", 110);
CONFIG_VALUE(J1Enabled, bool, "Judgment 1 Enabled", true);
CONFIG_VALUE(J1Text, std::string, "Judgment 2 Text", "Fantastic%n%s");
CONFIG_VALUE(J1Color, UnityEngine::Color, "Judgment 1 Color", UnityEngine::Color(0,1,0,1));

//Judgment 2
CONFIG_VALUE(J2Threshold, int, "Judgment 2 Threshold", 101);
CONFIG_VALUE(J2Enabled, bool, "Judgment 2 Enabled", true);
CONFIG_VALUE(J2Text, std::string, "Judgment 2 Text", "<size=80%>Excellent</size>%n%s");
CONFIG_VALUE(J2Color, UnityEngine::Color, "Judgment 2 Color", UnityEngine::Color(0.69,1,0,1));

//Judgment 3
CONFIG_VALUE(J3Threshold, int, "Judgment 3 Threshold", 90);
CONFIG_VALUE(J3Enabled, bool, "Judgment 3 Enabled", true);
CONFIG_VALUE(J3Text, std::string, "Judgment 3 Text", "<size=80%>Great</size>%n%s");
CONFIG_VALUE(J3Color, UnityEngine::Color, "Judgment 3 Color", UnityEngine::Color(1, 0.98, 0, 1));

//Judgment 4
CONFIG_VALUE(J4Threshold, int, "Judgment 4 Threshold", 80);
CONFIG_VALUE(J4Enabled, bool, "Judgment 4 Enabled", true);
CONFIG_VALUE(J4Text, std::string, "Judgment 4 Text", "<size=80%>Good</size>%n%s");
CONFIG_VALUE(J4Color, UnityEngine::Color, "Judgment 4 Color", UnityEngine::Color(1, 0.6, 0, 1));

//Judgment 5
CONFIG_VALUE(J5Threshold, int, "Judgment 5 Threshold", 60);
CONFIG_VALUE(J5Enabled, bool, "Judgment 5 Enabled", true);
CONFIG_VALUE(J5Text, std::string, "Judgment 5 Text", "<size=80%>Decent</size>%n%s");
CONFIG_VALUE(J5Color, UnityEngine::Color, "Judgment 5 Color", UnityEngine::Color(1,0,0,1));

//Judgment 6
CONFIG_VALUE(J6Threshold, int, "Judgment 6 Threshold", 0);
CONFIG_VALUE(J6Enabled, bool, "Judgment 6 Enabled", true);
CONFIG_VALUE(J6Text, std::string, "Judgment 6 Text", "<size=80%>Way Off</size>%n%s");
CONFIG_VALUE(J6Color, UnityEngine::Color, "Judgment 6 Color", UnityEngine::Color(0.5, 0, 0, 1));


    CONFIG_INIT_FUNCTION(
            //General Mod Config
            CONFIG_INIT_VALUE(IsEnabled);

            //Judgment 1
            CONFIG_INIT_VALUE(J1Threshold);
            CONFIG_INIT_VALUE(J1Enabled);
            CONFIG_INIT_VALUE(J1Text);
            CONFIG_INIT_VALUE(J1Color);

            //Judgment 2
            CONFIG_INIT_VALUE(J2Threshold);
            CONFIG_INIT_VALUE(J2Enabled);
            CONFIG_INIT_VALUE(J2Text);
            CONFIG_INIT_VALUE(J2Color);

            //Judgment 3
            CONFIG_INIT_VALUE(J3Threshold);
            CONFIG_INIT_VALUE(J3Enabled);
            CONFIG_INIT_VALUE(J3Text);
            CONFIG_INIT_VALUE(J3Color);

            //Judgment 4
            CONFIG_INIT_VALUE(J4Threshold);
            CONFIG_INIT_VALUE(J4Enabled);
            CONFIG_INIT_VALUE(J4Text);
            CONFIG_INIT_VALUE(J4Color);

            //Judgment 5
            CONFIG_INIT_VALUE(J5Threshold);
            CONFIG_INIT_VALUE(J5Enabled);
            CONFIG_INIT_VALUE(J5Text);
            CONFIG_INIT_VALUE(J5Color);

            //Judgment 6
            CONFIG_INIT_VALUE(J6Threshold);
            CONFIG_INIT_VALUE(J6Enabled);
            CONFIG_INIT_VALUE(J6Text);
            CONFIG_INIT_VALUE(J6Color);
    )
)