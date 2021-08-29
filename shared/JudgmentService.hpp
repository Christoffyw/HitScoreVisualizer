#pragma once

// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "modloader/shared/modloader.hpp"
// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/byref.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Color.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/register.hpp"
#include "TMPro/TextMeshPro.hpp"

#include "GlobalNamespace/FlyingScoreEffect.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounter.hpp"
#include <string>
#include <iostream>

namespace HitScore {
    class Judgment {
        public:
            int threshold;
            std::string text;
            UnityEngine::Color color;
            bool enabled;
    };
}

DECLARE_CLASS_CODEGEN(HitScore, JudgmentService, UnityEngine::MonoBehaviour,
    public:
        std::string DisplayModeFormat(int score, int before, int after, int accuracy, float timeDependence, const Judgment& judgment);
        std::string ConvertTimeDependencePrecision(float timeDependence, int decimalOffset, int decimalPrecision);
        std::string JudgeText(int score, int before, int after, int accuracy, float timeDependence);
        UnityEngine::Color JudgeColor(int score, int before, int after, int accuracy, float timeDependence);
        inline static std::vector< HitScore::Judgment > judgments;

    DECLARE_CTOR(ctor);
)



Configuration& getConfig();
Logger& getLogger();