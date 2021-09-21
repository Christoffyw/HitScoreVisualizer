#pragma once

// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "modloader/shared/modloader.hpp"

// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/register.hpp"
#include "TMPro/TextMeshPro.hpp"

#include "GlobalNamespace/FlyingScoreEffect.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounter.hpp"
#include "hooks.hpp"
#include "PluginConfig.hpp"

#include <map>

// Define these functions here so that we can easily read configuration and log information from other files


using namespace UnityEngine;

class HSV {
    public:
        static HSVConfig config;
        static bool configValid;
        static void loadConfig();
        static std::optional<int> getBestJudgment(std::vector<Judgment>& judgments, int comparison);
        static std::optional<const Segment> getBestSegment(std::vector<Segment>& segments, int comparison);
        static std::optional<const TimeSegment> getBestTimeSegment(std::vector<TimeSegment>& segments, int comparison);
        static void checkJudgments(GlobalNamespace::FlyingScoreEffect* flyingScoreEffect, int total, int before, int after, int accuracy, float timeDependence);
        static std::string DisplayModeFormat(int score, int before, int after, int accuracy, float timeDependence, const Judgment& judgment);
        static std::string ConvertTimeDependencePrecision(float timeDependence, int decimalOffset, int decimalPrecision);
        static std::string JudgeTimeSegment(int scoreForSegment, std::vector<TimeSegment> judgments);
        static std::string JudgeSegment(int scoreForSegment, std::vector<Segment> judgments);
        static std::string JudgeText(int score, int before, int after, int accuracy, float timeDependence, int bestIndex);
        static UnityEngine::Color JudgeColor(int score, int before, int after, int accuracy, float timeDependence, int bestIndex);
};

Configuration& getConfig();
Logger& getLogger();
const ModInfo& getModInfo();