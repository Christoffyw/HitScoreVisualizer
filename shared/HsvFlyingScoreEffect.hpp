#pragma once

// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "modloader/shared/modloader.hpp"
#include "main.hpp"
#include "JudgmentService.hpp"
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

#define GET_FIND_METHOD(mPtr) il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::get()

using namespace UnityEngine;

DECLARE_CLASS_CODEGEN(HitScore, HsvFlyingScoreEffect, GlobalNamespace::FlyingScoreEffect,
    DECLARE_INSTANCE_FIELD(HitScore::JudgmentService*, _judgmentService);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::NoteCutInfo, _noteCutInfo);
    DECLARE_INSTANCE_METHOD(void, Construct, HitScore::JudgmentService* judgmentService);
    DECLARE_INSTANCE_METHOD(void, Init, GlobalNamespace::NoteCutInfo noteCutInfo, int multiplier, float duration, Vector3 targetPos, Quaternion rotation, Color color);
    DECLARE_INSTANCE_METHOD(void, Judge, GlobalNamespace::ISaberSwingRatingCounter* swingRatingCounter, int assumedAfterCutScore);
)

Configuration& getConfig();
Logger& getLogger();

