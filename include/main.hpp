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
#include "GlobalNamespace/SaberSwingRatingCounter.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounterDidFinishReceiver.hpp"

#include "hooks.hpp"
#include <map>

// Define these functions here so that we can easily read configuration and log information from other files
#define GET_FIND_METHOD(mPtr) il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::get()

using namespace UnityEngine;

struct swingRatingCounter_context {
    GlobalNamespace::NoteCutInfo noteCutInfo;
    GlobalNamespace::FlyingScoreEffect* flyingScoreEffect;
};

DECLARE_CLASS_CODEGEN(HitScore, Main, MonoBehaviour,
    DECLARE_INSTANCE_METHOD(void, Update);
)

/*DECLARE_CLASS_INTERFACES(HitScore, EffectFinishHandler, "System", "Object", sizeof(Il2CppObject), { classof(GlobalNamespace::ISaberSwingRatingCounterDidFinishReceiver*) },
    DECLARE_OVERRIDE_METHOD(void, HandleSaberSwingRatingCounterDidFinish, GET_FIND_METHOD(&GlobalNamespace::ISaberSwingRatingCounterDidFinishReceiver::HandleSaberSwingRatingCounterDidFinish), GlobalNamespace::ISaberSwingRatingCounter* saberSwingRatingCounter);

    public:
        std::function<void(GlobalNamespace::ISaberSwingRatingCounter*)> swingRatingCounterFunction;
)*/


Configuration& getConfig();
Logger& getLogger();