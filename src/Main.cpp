#include "Main.hpp"

#include "GlobalNamespace/BeatmapObjectExecutionRating.hpp"
#include "GlobalNamespace/FlyingScoreEffect.hpp"
#include "GlobalNamespace/FlyingScoreSpawner.hpp"
#include "GlobalNamespace/IReadonlyCutScoreBuffer.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "Settings.hpp"
#include "TMPro/TextMeshPro.hpp"
#include "UnityEngine/AnimationCurve.hpp"
#include "UnityEngine/SpriteRenderer.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "bsml/shared/BSML.hpp"
#include "json/DefaultConfig.hpp"

static modloader::ModInfo modInfo = {MOD_ID, VERSION, 0};

std::string GlobalConfigPath() {
    static std::string path = Configuration::getConfigFilePath(modInfo);
    return path;
}

std::string ConfigsPath() {
    static std::string path = getDataDir(modInfo);
    return path;
}

bool LoadCurrentConfig() {
    if (!fileexists(globalConfig.SelectedConfig)) {
        logger.warn("Could not find selected config!");
        writefile(ConfigsPath() + defaultConfigName, defaultConfigText);
        globalConfig.SelectedConfig = ConfigsPath() + defaultConfigName;
        WriteToFile(GlobalConfigPath(), globalConfig);
    }
    HSV::Config config;
    bool retry = false;
    do {
        try {
            ReadFromFile(globalConfig.SelectedConfig, config);
            globalConfig.CurrentConfig = config;
            return true;
        } catch (std::exception const& err) {
            logger.error("Could not load config file {}: {}", globalConfig.SelectedConfig, err.what());
            if (config.IsDefault) {
                writefile(globalConfig.SelectedConfig, defaultConfigText);
                retry = !retry;
            } else
                return false;
        }
    } while (retry);
    return false;
}

// global config
HSV::GlobalConfig globalConfig{};
// used for fixed position
GlobalNamespace::FlyingScoreEffect* currentEffect = nullptr;
// used for updating ratings
std::unordered_map<GlobalNamespace::IReadonlyCutScoreBuffer*, GlobalNamespace::FlyingScoreEffect*> swingRatingMap = {};

MAKE_HOOK_MATCH(
    FlyingScoreEffect_InitAndPresent,
    &GlobalNamespace::FlyingScoreEffect::InitAndPresent,
    void,
    GlobalNamespace::FlyingScoreEffect* self,
    GlobalNamespace::IReadonlyCutScoreBuffer* cutScoreBuffer,
    float duration,
    UnityEngine::Vector3 targetPos,
    UnityEngine::Color color
) {
    if (globalConfig.GetActive()) {
        if (globalConfig.CurrentConfig->FixedPos) {
            targetPos = globalConfig.CurrentConfig->FixedPos.value();
            self->transform->position = targetPos;
            if (!globalConfig.HideUntilDone) {
                if (currentEffect)
                    currentEffect->gameObject->active = false;
                currentEffect = self;
            }
        } else if (globalConfig.CurrentConfig->PosOffset) {
            targetPos = UnityEngine::Vector3::op_Addition(targetPos, globalConfig.CurrentConfig->PosOffset.value());
        }
    }
    FlyingScoreEffect_InitAndPresent(self, cutScoreBuffer, duration, targetPos, color);

    if (globalConfig.GetActive()) {
        if (cutScoreBuffer == nullptr) {
            logger.error("CutScoreBuffer is null!");
            return;
        }
        auto noteCutInfo = cutScoreBuffer->noteCutInfo;
        if (noteCutInfo.noteData->scoringType == GlobalNamespace::NoteData::ScoringType::BurstSliderHead && !globalConfig.CurrentConfig->HasChainHead)
            return;
        if (noteCutInfo.noteData->scoringType == GlobalNamespace::NoteData::ScoringType::BurstSliderElement &&
            !globalConfig.CurrentConfig->HasChainLink)
            return;

        if (!cutScoreBuffer->isFinished)
            swingRatingMap.insert({self->_cutScoreBuffer, self});

        self->_maxCutDistanceScoreIndicator->enabled = false;
        self->_text->richText = true;
        self->_text->enableWordWrapping = false;
        self->_text->overflowMode = TMPro::TextOverflowModes::Overflow;

        Judge((GlobalNamespace::CutScoreBuffer*) cutScoreBuffer, self, noteCutInfo);
    }
}

MAKE_HOOK_MATCH(
    CutScoreBuffer_HandleSaberSwingRatingCounterDidChange,
    &GlobalNamespace::CutScoreBuffer::HandleSaberSwingRatingCounterDidChange,
    void,
    GlobalNamespace::CutScoreBuffer* self,
    GlobalNamespace::ISaberSwingRatingCounter* swingRatingCounter,
    float rating
) {
    CutScoreBuffer_HandleSaberSwingRatingCounterDidChange(self, swingRatingCounter, rating);

    if (globalConfig.GetActive()) {
        auto noteCutInfo = self->noteCutInfo;
        if (noteCutInfo.noteData->scoringType == GlobalNamespace::NoteData::ScoringType::BurstSliderHead && !globalConfig.CurrentConfig->HasChainHead)
            return;
        if (noteCutInfo.noteData->scoringType == GlobalNamespace::NoteData::ScoringType::BurstSliderElement &&
            !globalConfig.CurrentConfig->HasChainLink)
            return;

        auto itr = swingRatingMap.find((GlobalNamespace::IReadonlyCutScoreBuffer*) self);
        if (itr == swingRatingMap.end()) {
            logger.error("Counter was not found in swingRatingMap!");
            return;
        }
        auto& flyingScoreEffect = itr->second;

        Judge(self, flyingScoreEffect, noteCutInfo);
    }
}

MAKE_HOOK_MATCH(
    CutScoreBuffer_HandleSaberSwingRatingCounterDidFinish,
    &GlobalNamespace::CutScoreBuffer::HandleSaberSwingRatingCounterDidFinish,
    void,
    GlobalNamespace::CutScoreBuffer* self,
    GlobalNamespace::ISaberSwingRatingCounter* swingRatingCounter
) {
    CutScoreBuffer_HandleSaberSwingRatingCounterDidFinish(self, swingRatingCounter);

    if (globalConfig.GetActive()) {
        auto noteCutInfo = self->noteCutInfo;
        if (noteCutInfo.noteData->scoringType == GlobalNamespace::NoteData::ScoringType::BurstSliderHead && !globalConfig.CurrentConfig->HasChainHead)
            return;
        if (noteCutInfo.noteData->scoringType == GlobalNamespace::NoteData::ScoringType::BurstSliderElement &&
            !globalConfig.CurrentConfig->HasChainLink)
            return;

        auto itr = swingRatingMap.find((GlobalNamespace::IReadonlyCutScoreBuffer*) self);
        if (itr == swingRatingMap.end()) {
            logger.error("Counter was not found in swingRatingMap!");
            return;
        }
        auto flyingScoreEffect = itr->second;
        swingRatingMap.erase(itr);

        Judge(self, flyingScoreEffect, noteCutInfo);

        if (globalConfig.CurrentConfig->FixedPos && globalConfig.HideUntilDone) {
            if (currentEffect)
                currentEffect->gameObject->active = false;
            currentEffect = flyingScoreEffect;
        }
    }
}

MAKE_HOOK_MATCH(
    FlyingScoreSpawner_HandleFlyingObjectEffectDidFinish,
    &GlobalNamespace::FlyingScoreSpawner::HandleFlyingObjectEffectDidFinish,
    void,
    GlobalNamespace::FlyingScoreSpawner* self,
    GlobalNamespace::FlyingObjectEffect* effect
) {

    if (currentEffect == (GlobalNamespace::FlyingScoreEffect*) effect) {
        currentEffect->gameObject->active = false;
        currentEffect = nullptr;
    }
    FlyingScoreSpawner_HandleFlyingObjectEffectDidFinish(self, effect);
}

MAKE_HOOK_MATCH(
    FlyingScoreEffect_ManualUpdate, &GlobalNamespace::FlyingScoreEffect::ManualUpdate, void, GlobalNamespace::FlyingScoreEffect* self, float t
) {
    FlyingScoreEffect_ManualUpdate(self, t);

    if (globalConfig.GetActive()) {
        self->_color.a = self->_fadeAnimationCurve->Evaluate(t);
        self->_text->color = self->_color;
    }
}

extern "C" void setup(CModInfo* info) {
    *info = modInfo.to_c();

    Paper::Logger::RegisterFileContextId(MOD_ID);

    if (!direxists(ConfigsPath()))
        mkpath(ConfigsPath());

    try {
        if (!fileexists(GlobalConfigPath()))
            WriteToFile(GlobalConfigPath(), globalConfig);
        else
            ReadFromFile(GlobalConfigPath(), globalConfig);
    } catch (std::exception const& err) {
        logger.error("Error loading global config: {}", err.what());
    }
    // load current config, or select default
    if (!LoadCurrentConfig()) {
        logger.error("Failed to load selected config! Loading default instead");
        writefile(ConfigsPath() + defaultConfigName, defaultConfigText);
        globalConfig.SelectedConfig = ConfigsPath() + defaultConfigName;
        WriteToFile(GlobalConfigPath(), globalConfig);
        LoadCurrentConfig();
    }

    logger.info("Completed setup!");
}

extern "C" void late_load() {
    il2cpp_functions::Init();

    BSML::Register::RegisterSettingsMenu<HSV::SettingsViewController*>("Hit Score Visualizer");
    BSML::Register::RegisterMainMenu<HSV::SettingsViewController*>("Hit Score Visualizer", "Hit Score Visualizer", "");

    logger.info("Installing hooks...");
    INSTALL_HOOK(logger, FlyingScoreEffect_InitAndPresent);
    INSTALL_HOOK(logger, CutScoreBuffer_HandleSaberSwingRatingCounterDidChange);
    INSTALL_HOOK(logger, CutScoreBuffer_HandleSaberSwingRatingCounterDidFinish);
    INSTALL_HOOK(logger, FlyingScoreSpawner_HandleFlyingObjectEffectDidFinish);
    INSTALL_HOOK(logger, FlyingScoreEffect_ManualUpdate);
    logger.info("Installed all hooks!");
}
