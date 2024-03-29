#include "Main.hpp"
#include "Settings.hpp"
#include "json/Config.hpp"
#include "json/DefaultConfig.hpp"

#include "GlobalNamespace/CutScoreBuffer.hpp"
#include "GlobalNamespace/IReadonlyCutScoreBuffer.hpp"
#include "GlobalNamespace/BeatmapObjectExecutionRating.hpp"
#include "GlobalNamespace/FlyingScoreEffect_Pool.hpp"
#include "GlobalNamespace/FlyingScoreSpawner.hpp"

#include "TMPro/TextMeshPro.hpp"

#include "UnityEngine/AnimationCurve.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/SpriteRenderer.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"

#include "questui/shared/QuestUI.hpp"

#include <filesystem>

using namespace GlobalNamespace;
using namespace HSV;

static ModInfo modInfo;

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

std::string GlobalConfigPath() {
    static std::string path = Configuration::getConfigFilePath(modInfo);
    return path;
}

std::string ConfigsPath() {
    static std::string path = getDataDir(modInfo);
    return path;
}

bool LoadCurrentConfig() {
    if(!fileexists(globalConfig.SelectedConfig)) {
        LOG_ERROR("Could not find selected config!");
        writefile(ConfigsPath() + defaultConfigName, defaultConfigText);
        globalConfig.SelectedConfig = ConfigsPath() + defaultConfigName;
        WriteToFile(GlobalConfigPath(), globalConfig);
    }
    Config config;
    bool retry = false;
    do {
        try {
            ReadFromFile(globalConfig.SelectedConfig, config);
            globalConfig.CurrentConfig = config;
            return true;
        } catch(const std::exception& err) {
            LOG_ERROR("Could not load config file %s: %s", globalConfig.SelectedConfig.c_str(), err.what());
            if(config.IsDefault) {
                writefile(globalConfig.SelectedConfig, defaultConfigText);
                retry = !retry;
            } else
                return false;
        }
    } while(retry);
    return false;
}

// global config
GlobalConfig globalConfig{};
// used for fixed position
FlyingScoreEffect* currentEffect = nullptr;
// used for updating ratings
std::unordered_map<IReadonlyCutScoreBuffer*, FlyingScoreEffect*> swingRatingMap = {};

MAKE_HOOK_MATCH(InitFlyingScoreEffect, &FlyingScoreEffect::InitAndPresent,
        void, FlyingScoreEffect* self, IReadonlyCutScoreBuffer* cutScoreBuffer, float duration, UnityEngine::Vector3 targetPos, UnityEngine::Color color) {

    if(globalConfig.GetActive()) {
        if(globalConfig.CurrentConfig->FixedPos) {
            targetPos = globalConfig.CurrentConfig->FixedPos.value();
            self->get_transform()->set_position(targetPos);
            if(!globalConfig.HideUntilDone) {
                if(currentEffect)
                    currentEffect->get_gameObject()->SetActive(false);
                currentEffect = self;
            }
        } else if(globalConfig.CurrentConfig->PosOffset) {
            targetPos = targetPos + globalConfig.CurrentConfig->PosOffset.value();
        }
    }
    InitFlyingScoreEffect(self, cutScoreBuffer, duration, targetPos, color);

    if(globalConfig.GetActive()) {
        if(cutScoreBuffer == nullptr) {
            LOG_ERROR("CutScoreBuffer is null");
            return;
        }
        auto noteCutInfo = cutScoreBuffer->get_noteCutInfo();
        if(noteCutInfo.noteData->scoringType == NoteData::ScoringType::BurstSliderHead && !globalConfig.CurrentConfig->HasChainHead)
            return;
        if(noteCutInfo.noteData->scoringType == NoteData::ScoringType::BurstSliderElement && !globalConfig.CurrentConfig->HasChainLink)
            return;

        if(!cutScoreBuffer->get_isFinished())
            swingRatingMap.insert({self->cutScoreBuffer, self});

        self->maxCutDistanceScoreIndicator->set_enabled(false);
        self->text->set_richText(true);
        self->text->set_enableWordWrapping(false);
        self->text->set_overflowMode(TMPro::TextOverflowModes::Overflow);

        Judge((CutScoreBuffer*) cutScoreBuffer, self, noteCutInfo);
    }
}

MAKE_HOOK_MATCH(HandleSwingChange, &CutScoreBuffer::HandleSaberSwingRatingCounterDidChange,
        void, CutScoreBuffer* self, ISaberSwingRatingCounter* swingRatingCounter, float rating) {

    HandleSwingChange(self, swingRatingCounter, rating);

    if(globalConfig.GetActive()) {
        auto noteCutInfo = self->get_noteCutInfo();
        if(noteCutInfo.noteData->scoringType == NoteData::ScoringType::BurstSliderHead && !globalConfig.CurrentConfig->HasChainHead)
            return;
        if(noteCutInfo.noteData->scoringType == NoteData::ScoringType::BurstSliderElement && !globalConfig.CurrentConfig->HasChainLink)
            return;

        auto itr = swingRatingMap.find((IReadonlyCutScoreBuffer*) self);
        if(itr == swingRatingMap.end()) {
            LOG_ERROR("Counter was not found in swingRatingMap!");
            return;
        }
        auto& flyingScoreEffect = itr->second;

        Judge(self, flyingScoreEffect, noteCutInfo);
    }
}

MAKE_HOOK_MATCH(HandleSwingFinish, &CutScoreBuffer::HandleSaberSwingRatingCounterDidFinish,
        void, CutScoreBuffer* self, ISaberSwingRatingCounter* swingRatingCounter) {

    HandleSwingFinish(self, swingRatingCounter);

    if(globalConfig.GetActive()) {
        auto noteCutInfo = self->get_noteCutInfo();
        if(noteCutInfo.noteData->scoringType == NoteData::ScoringType::BurstSliderHead && !globalConfig.CurrentConfig->HasChainHead)
            return;
        if(noteCutInfo.noteData->scoringType == NoteData::ScoringType::BurstSliderElement && !globalConfig.CurrentConfig->HasChainLink)
            return;

        auto itr = swingRatingMap.find((IReadonlyCutScoreBuffer*) self);
        if(itr == swingRatingMap.end()) {
            LOG_ERROR("Counter was not found in swingRatingMap!");
            return;
        }
        auto flyingScoreEffect = itr->second;
        swingRatingMap.erase(itr);

        Judge(self, flyingScoreEffect, noteCutInfo);

        if(globalConfig.CurrentConfig->FixedPos && globalConfig.HideUntilDone) {
            if(currentEffect)
                currentEffect->get_gameObject()->SetActive(false);
            currentEffect = flyingScoreEffect;
        }
    }
}

MAKE_HOOK_MATCH(FlyingScoreEffectFinish, &FlyingScoreSpawner::HandleFlyingObjectEffectDidFinish, void, FlyingScoreSpawner* self, FlyingObjectEffect* effect) {

    if(currentEffect == (FlyingScoreEffect*) effect) {
        currentEffect->get_gameObject()->SetActive(false);
        currentEffect = nullptr;
    }
    FlyingScoreEffectFinish(self, effect);
}

MAKE_HOOK_MATCH(FlyingScoreEffectManualUpdate, &FlyingScoreEffect::ManualUpdate,
        void, FlyingScoreEffect* self, float t) {

    FlyingScoreEffectManualUpdate(self, t);

    if(globalConfig.GetActive()) {
        self->color.a = self->fadeAnimationCurve->Evaluate(t);
        self->text->set_color(self->color);
    }
}

extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;

    if(!direxists(ConfigsPath()))
        mkpath(ConfigsPath());

    try {
        if(!fileexists(GlobalConfigPath()))
            WriteToFile(GlobalConfigPath(), globalConfig);
        else
            ReadFromFile(GlobalConfigPath(), globalConfig);
    } catch(const std::exception& err) {
        LOG_ERROR("Error loading global config: %s", err.what());
    }
    // load current config, or select default
    if(!LoadCurrentConfig()) {
        LOG_ERROR("Failed to load selected config! Loading default instead");
        writefile(ConfigsPath() + defaultConfigName, defaultConfigText);
        globalConfig.SelectedConfig = ConfigsPath() + defaultConfigName;
        WriteToFile(GlobalConfigPath(), globalConfig);
        LoadCurrentConfig();
    }

    LOG_INFO("Completed setup!");
}

extern "C" void load() {
    il2cpp_functions::Init();

    QuestUI::Register::RegisterModSettingsViewController<SettingsViewController*>(modInfo, "Hit Score Visualizer");
    QuestUI::Register::RegisterMainMenuModSettingsViewController<SettingsViewController*>(modInfo, "Hit Score Visualizer");

    LOG_INFO("Installing hooks...");
    INSTALL_HOOK(getLogger(), InitFlyingScoreEffect);
    INSTALL_HOOK(getLogger(), HandleSwingChange);
    INSTALL_HOOK(getLogger(), HandleSwingFinish);
    INSTALL_HOOK(getLogger(), FlyingScoreEffectFinish);
    INSTALL_HOOK(getLogger(), FlyingScoreEffectManualUpdate);
    LOG_INFO("Installed all hooks!");
}
