#pragma once

// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "modloader/shared/modloader.hpp"
#include "main.hpp"
// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/byref.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/register.hpp"
#include "TMPro/TextMeshPro.hpp"

#include "GlobalNamespace/FlyingScoreEffect.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounter.hpp"

DECLARE_CLASS_CODEGEN(HitScore, JudgmentService, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_METHOD(Il2CppString*, JudgeText, int score, int before, int after, int accuracy, float timeDependence);
    DECLARE_INSTANCE_METHOD(UnityEngine::Color, JudgeColor, int score, int before, int after, int accuracy, float timeDependence);
    DECLARE_INSTANCE_METHOD(Il2CppString*, DisplayModeFormat, int score, int before, int after, int accuracy, float timeDependence);
)

DECLARE_CLASS_CODEGEN(HitScore, Judgment, UnityEngine::MonoBehaviour,
int threshold;
Il2CppString* text;
UnityEngine::Color color;
bool fade;
)

Configuration& getConfig();
Logger& getLogger();