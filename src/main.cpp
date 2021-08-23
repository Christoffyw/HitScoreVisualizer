#include "PluginConfig.hpp"
#include "main.hpp"
#include "JudgmentService.hpp"
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
#include "GlobalNamespace/FlyingScoreEffect_Pool.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounter.hpp"
#include "GlobalNamespace/ScoreModel.hpp"
#include "GlobalNamespace/FlyingScoreSpawner.hpp"
#include "GlobalNamespace/BeatmapObjectExecutionRatingsRecorder_CutScoreHandler.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/byref.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/register.hpp"
#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/IMemoryPool.hpp"
#include "Zenject/MemoryPoolBase_1.hpp"
#include "Zenject/MonoMemoryPool_1.hpp"
#include "System/Action_1.hpp"
#include <iostream>
#include <string>
#include <utility>

using namespace std;
using namespace HMUI;
using namespace GlobalNamespace;
using namespace QuestUI;
using namespace UnityEngine;

DEFINE_TYPE(HitScore, Main);

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration configuration(modInfo);
    return configuration;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;

    getConfig().Load();
    getPluginConfig().Init(info);
    getConfig().Reload();
    getConfig().Write();

    getLogger().info("Completed setup!");
}

HitScore::JudgmentService* judgmentService;
FlyingScoreEffect* currentEffect = nullptr;
std::map<GlobalNamespace::ISaberSwingRatingCounter*, swingRatingCounter_context> swingRatingMap;

void JudgeNoContext(FlyingScoreEffect* self, NoteCutInfo& noteCutInfo) {
    int before;
    int after;
    int accuracy;
    ScoreModel::RawScoreWithoutMultiplier(noteCutInfo.swingRatingCounter, noteCutInfo.cutDistanceToCenter, byref(before), byref(after), byref(accuracy));
    int total = before + after + accuracy;
    float timeDependence = Mathf::Abs(noteCutInfo.cutNormal.z);
    self->text->set_text(il2cpp_utils::newcsstr(judgmentService->JudgeText(total, before, after, accuracy, timeDependence)));
    self->text->set_color(judgmentService->JudgeColor(total, before, after, accuracy, timeDependence));
    self->color = judgmentService->JudgeColor(total, before, after, accuracy, timeDependence);
    //noteCutInfo.swingRatingCounter->UnregisterDidFinishReceiver(reinterpret_cast<GlobalNamespace::ISaberSwingRatingCounterDidFinishReceiver*>(hsvRatingCounter));
}

void Judge(ISaberSwingRatingCounter* counter) {

    auto itr = swingRatingMap.find(counter);
    if(itr == swingRatingMap.end()) {
        getLogger().info("counter was not found in swingRatingMap!");
        return;
    }
    auto context = itr->second;

    int before;
    int after;
    int accuracy;


    ScoreModel::RawScoreWithoutMultiplier(context.noteCutInfo.swingRatingCounter, context.noteCutInfo.cutDistanceToCenter, byref(before), byref(after), byref(accuracy));
    int total = before + after + accuracy;
    float timeDependence = Mathf::Abs(context.noteCutInfo.cutNormal.z);
    if(context.flyingScoreEffect) {
        //context.flyingScoreEffect->maxCutDistanceScoreIndicator->set_enabled(true);
        context.flyingScoreEffect->text->set_text(il2cpp_utils::newcsstr(judgmentService->JudgeText(total, before, after, accuracy, timeDependence)));
        context.flyingScoreEffect->text->set_color(judgmentService->JudgeColor(total, before, after, accuracy, timeDependence));
        context.flyingScoreEffect->color = judgmentService->JudgeColor(total, before, after, accuracy, timeDependence);
        swingRatingMap.erase(itr);
    }
    //noteCutInfo.swingRatingCounter->UnregisterDidFinishReceiver(reinterpret_cast<GlobalNamespace::ISaberSwingRatingCounterDidFinishReceiver*>(hsvRatingCounter));
}

MAKE_HOOK_MATCH(InitFlyingScoreEffect, &FlyingScoreEffect::InitAndPresent, void, FlyingScoreEffect* self, ByRef<NoteCutInfo> noteCutInfo, int multiplier, float duration, Vector3 targetPos, Quaternion rotation, Color color) {
    if(getPluginConfig().IsEnabled.GetValue()) {
        if(currentEffect) {
            currentEffect->duration = 0;
        }
        currentEffect = self;


    }

    InitFlyingScoreEffect(self, noteCutInfo, multiplier, duration, targetPos, rotation, color);

    if(getPluginConfig().IsEnabled.GetValue()) {
        duration = 0.7;
        self->text->set_text(il2cpp_utils::newcsstr(""));
        self->maxCutDistanceScoreIndicator->set_enabled(false);

        //
        //hsvRatingCounter->swingRatingCounterFunction = [=](GlobalNamespace::ISaberSwingRatingCounter* saber) { Judge(self, noteCutInfo.heldRef, hsvRatingCounter); };

        //JudgeNoContext(self, noteCutInfo.heldRef);

        auto counter = noteCutInfo.heldRef.swingRatingCounter;

        GlobalNamespace::NoteCutInfo info = noteCutInfo.heldRef;

        swingRatingMap.insert(std::make_pair(counter, swingRatingCounter_context{info, self}));



        //noteCutInfo.heldRef.swingRatingCounter->UnregisterDidFinishReceiver(reinterpret_cast<GlobalNamespace::ISaberSwingRatingCounterDidFinishReceiver*>(hsvRatingCounter));
        //noteCutInfo.heldRef.swingRatingCounter->RegisterDidFinishReceiver(reinterpret_cast<GlobalNamespace::ISaberSwingRatingCounterDidFinishReceiver*>(hsvRatingCounter));
    }
}

MAKE_HOOK_MATCH(HandleSwingFinish, &GlobalNamespace::BeatmapObjectExecutionRatingsRecorder_CutScoreHandler::HandleSaberSwingRatingCounterDidFinish, void, GlobalNamespace::BeatmapObjectExecutionRatingsRecorder_CutScoreHandler* self, ISaberSwingRatingCounter* counter) {
    HandleSwingFinish(self, counter);
    Judge(counter);
}

MAKE_HOOK_MATCH(FlyingScoreEffectHook, &GlobalNamespace::EffectPoolsManualInstaller::ManualInstallBindings, void, GlobalNamespace::EffectPoolsManualInstaller* self, Zenject::DiContainer* diContainer, bool shortBeatEffect) {
    GameObject* gameObject = self->flyingScoreEffectPrefab->get_gameObject();

    FlyingScoreEffect* flyingScoreEffect = gameObject->GetComponent<FlyingScoreEffect*>();

    if(gameObject != nullptr) {

        FlyingObjectEffect* flyingObjectEffect = flyingScoreEffect;

        if(flyingScoreEffect != nullptr) {
            judgmentService = CRASH_UNLESS(il2cpp_utils::New<HitScore::JudgmentService*>());
        }
    }

    FlyingScoreEffectHook(self, diContainer, shortBeatEffect);
}

MAKE_HOOK_MATCH(FlyingScoreEffectFinish, &GlobalNamespace::FlyingScoreSpawner::HandleFlyingObjectEffectDidFinish, void, FlyingScoreSpawner* self, FlyingObjectEffect* flyingObjectEffect) {
    FlyingScoreEffectFinish(self, flyingObjectEffect);
    if(flyingObjectEffect == currentEffect) {
        currentEffect = nullptr;
    }
}

void HitScore::Main::Update() {

}

int value = 0;

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    getLogger().info("DidActivate: %p, %d, %d, %d", self, firstActivation, addedToHierarchy, screenSystemEnabling);





    if(firstActivation) {
        self->get_gameObject()->AddComponent<HMUI::Touchable*>();
        self->get_gameObject()->AddComponent<HitScore::Main*>();
        GameObject* container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());

        auto* textGrid = container;
        //        textGrid->set_spacing(1);

        QuestUI::BeatSaberUI::CreateText(textGrid->get_transform(), "Hit Score Visualizer settings.");
        QuestUI::BeatSaberUI::AddHoverHint(AddConfigValueToggle(textGrid->get_transform(), getPluginConfig().IsEnabled)->get_gameObject(),"Toggles whether the mod is active or not.");

        //        buttonsGrid->set_spacing(1);

        //QuestUI::BeatSaberUI::AddHoverHint(AddConfigValueIncrementFloat(textGrid->get_transform(), getPluginConfig().MaxSpeed, 0, 10, 30, 1000)->get_gameObject(),"Changes the speed of scrolling with the joystick. Default: 600");
        //QuestUI::BeatSaberUI::AddHoverHint(AddConfigValueIncrementFloat(textGrid->get_transform(), getPluginConfig().Accel, 1, 0.1, 0.5, 5)->get_gameObject(),"Changes the acceleration speed of scrolling with the joystick. Default: 1.5");
        //QuestUI::BeatSaberUI::AddHoverHint(AddConfigValueToggle(textGrid->get_transform(), getPluginConfig().IsLinear)->get_gameObject(),"Toggles whether you want to use acceleration or not.");
    }

}

extern "C" void load() {
    il2cpp_functions::Init();
    custom_types::Register::AutoRegister();
    QuestUI::Init();
    QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);
    INSTALL_HOOK(getLogger(), InitFlyingScoreEffect);
    INSTALL_HOOK(getLogger(), FlyingScoreEffectHook);
    INSTALL_HOOK(getLogger(), HandleSwingFinish);
    INSTALL_HOOK(getLogger(), FlyingScoreEffectFinish);
}