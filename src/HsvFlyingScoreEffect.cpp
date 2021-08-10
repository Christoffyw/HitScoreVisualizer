#include "HsvFlyingScoreEffect.hpp"
#include "PluginConfig.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/CanvasScaler.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/CanvasRenderer.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Canvas.hpp"
#include "UnityEngine/RenderMode.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/SpriteRenderer.hpp"
#include "UnityEngine/AnimationCurve.hpp"

#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/ViewController.hpp"
#include "HMUI/CurvedCanvasSettings.hpp"
#include "HMUI/Touchable.hpp"
#include "HMUI/ScrollView.hpp"
#include "HMUI/TableView.hpp"

#include "TMPro/TextMeshProUGUI.hpp"
#include "TMPro/TextMeshPro.hpp"

#include "GlobalNamespace/LevelCollectionTableView.hpp"
#include "GlobalNamespace/HapticFeedbackController.hpp"
#include "GlobalNamespace/EffectPoolsManualInstaller.hpp"
#include "GlobalNamespace/FlyingScoreEffect.hpp"
#include "GlobalNamespace/FlyingObjectEffect.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounter.hpp"
#include "GlobalNamespace/ScoreModel.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/byref.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/register.hpp"
#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "Zenject/DiContainer.hpp"
#include <iostream>
#include <string>

using namespace std;
using namespace HMUI;
using namespace GlobalNamespace;
using namespace QuestUI;
using namespace UnityEngine;

DEFINE_TYPE(HitScore, HsvFlyingScoreEffect);

void HitScore::HsvFlyingScoreEffect::Construct(HitScore::JudgmentService *judgmentService) {
    _judgmentService = judgmentService;
    getLogger().info("CONSTRUCTION COMPELTE");
}

MAKE_HOOK_MATCH(InitAndPresent, &FlyingScoreEffect::InitAndPresent, void, FlyingScoreEffect* self, ByRef<GlobalNamespace::NoteCutInfo> noteCutInfo, int multiplier, float duration, Vector3 targetPos, Quaternion rotation, Color color) {
    auto selfCast = il2cpp_utils::try_cast<HitScore::HsvFlyingScoreEffect>(self);
    if (selfCast) {
        selfCast.value()->Init(noteCutInfo.heldRef, multiplier, duration, targetPos, rotation, color);
    } else {
        InitAndPresent(self, noteCutInfo, multiplier, duration, targetPos, rotation, color);
    }
}

void HitScore::HsvFlyingScoreEffect::Init(GlobalNamespace::NoteCutInfo noteCutInfo, int multiplier, float duration, Vector3 targetPos, Quaternion rotation, Color color) {
    _noteCutInfo = noteCutInfo;
    getLogger().info("INIT ACTUALLY HAPPENED");
    _set__color(color);
    _set__saberSwingRatingCounter(noteCutInfo.swingRatingCounter);
    _set__cutDistanceToCenter(noteCutInfo.cutDistanceToCenter);
    //_get__saberSwingRatingCounter()->RegisterDidChangeReceiver(this);
    //_get__saberSwingRatingCounter()->RegisterDidFinishReceiver(this);
    _set__registeredToCallbacks(true);

    _get__maxCutDistanceScoreIndicator()->set_enabled(false);
    Judge(_noteCutInfo.swingRatingCounter, 30);
}

MAKE_HOOK_MATCH(HandleSwingChange, &FlyingScoreEffect::HandleSaberSwingRatingCounterDidChange, void, FlyingScoreEffect* self, GlobalNamespace::ISaberSwingRatingCounter* swingRatingCounter, float rating) {
    getLogger().info("Try to JUDGE Change");
    auto selfCast = il2cpp_utils::try_cast<HitScore::HsvFlyingScoreEffect>(self);
    if (selfCast) {
        selfCast.value()->Judge(swingRatingCounter, 0);
    } else {
        HandleSwingChange(self, swingRatingCounter, rating);
    }
}

MAKE_HOOK_MATCH(HandleSwingFinish, &FlyingScoreEffect::HandleSaberSwingRatingCounterDidFinish, void, FlyingScoreEffect* self, GlobalNamespace::ISaberSwingRatingCounter* swingRatingCounter) {
    getLogger().info("Try to JUDGE FInish");
    auto selfCast = il2cpp_utils::try_cast<HitScore::HsvFlyingScoreEffect>(self);
    if (selfCast) {
        selfCast.value()->Judge(swingRatingCounter, 0);
    } else {
        HandleSwingFinish(self, swingRatingCounter);
    }
}

void HitScore::HsvFlyingScoreEffect::Judge(GlobalNamespace::ISaberSwingRatingCounter *swingRatingCounter, int assumedAfterCutScore) {
    getLogger().info("JUDGE");
    int before;
    int after;
    int accuracy;
    ScoreModel::RawScoreWithoutMultiplier(swingRatingCounter, _get__cutDistanceToCenter(), before, after, accuracy);
    if(assumedAfterCutScore != 0)
        after = assumedAfterCutScore;
    int total = before + after + accuracy;
    float timeD = Mathf::Abs(_noteCutInfo.cutNormal.z);
    text->set_text(_judgmentService->JudgeText(total, before, after, accuracy, timeD));
    _set__color(_judgmentService->JudgeColor(total, before, after, accuracy, timeD));
}

void HitScore::InstallHooks() {
    INSTALL_HOOK(getLogger(), InitAndPresent);
    INSTALL_HOOK(getLogger(), HandleSwingChange);
    INSTALL_HOOK(getLogger(), HandleSwingFinish);
}