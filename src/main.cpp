#include "Main.hpp"
#include "json/Config.hpp"
#include "json/DefaultConfig.hpp"

#include "GlobalNamespace/BeatmapObjectExecutionRatingsRecorder_CutScoreHandler.hpp"
#include "GlobalNamespace/ColorExtensions.hpp"

#include "TMPro/TextMeshPro.hpp"

#include "UnityEngine/AnimationCurve.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/SpriteRenderer.hpp"
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
    if(!fileexists(ConfigsPath() + globalConfig.SelectedConfig)) {
        LOG_ERROR("Could not find selected config!");
        writefile(ConfigsPath() + defaultConfigName, defaultConfigText);
        globalConfig.SelectedConfig = defaultConfigName;
        WriteToFile(GlobalConfigPath(), globalConfig);
    }
    try {
        Config config;
        ReadFromFile(ConfigsPath() + globalConfig.SelectedConfig, config);
        globalConfig.CurrentConfig = config;
    } catch(std::runtime_error const& e) {
        LOG_ERROR("Error loading config: %s", e.what());
        return false;
    }
    return true;
}

// global config
GlobalConfig globalConfig{};
// used for fixed position
FlyingScoreEffect* currentEffect = nullptr;
// used for updating ratings
std::unordered_map<ISaberSwingRatingCounter*, std::pair<NoteCutInfo, FlyingScoreEffect*>> swingRatingMap = {};

MAKE_HOOK_MATCH(InitFlyingScoreEffect, &FlyingScoreEffect::InitAndPresent,
        void, FlyingScoreEffect* self, ByRef<NoteCutInfo> noteCutInfo, int multiplier, float duration, UnityEngine::Vector3 targetPos, UnityEngine::Quaternion rotation, UnityEngine::Color color) {
    
    if(globalConfig.GetActive()) {
        if(globalConfig.CurrentConfig->PosOffset) {
            targetPos = targetPos + globalConfig.CurrentConfig->PosOffset.value();
        }
        else if(globalConfig.CurrentConfig->FixedPos) {
            targetPos = globalConfig.CurrentConfig->FixedPos.value();
            self->get_transform()->set_position(targetPos);
            currentEffect = self;
        }
    }

    InitFlyingScoreEffect(self, noteCutInfo, multiplier, duration, targetPos, rotation, color);

    if(globalConfig.GetActive()) {
        self->maxCutDistanceScoreIndicator->set_enabled(false);
        swingRatingMap.insert({self->saberSwingRatingCounter, {noteCutInfo.heldRef, self}});

        Judge(self->saberSwingRatingCounter, self, noteCutInfo.heldRef);
    }
}

MAKE_HOOK_MATCH(HandleSwingChange, &FlyingScoreEffect::HandleSaberSwingRatingCounterDidChange,
        void, FlyingScoreEffect* self, ISaberSwingRatingCounter* counter, float rating) {

    HandleSwingChange(self, counter, rating);

    if(globalConfig.GetActive()) {
        auto itr = swingRatingMap.find(counter);
        if(itr == swingRatingMap.end()) {
            LOG_ERROR("Counter was not found in swingRatingMap!");
            return;
        }
        auto& noteCutInfo = itr->second.first;
        auto& flyingScoreEffect = itr->second.second;

        Judge(counter, flyingScoreEffect, noteCutInfo);
    }
}

MAKE_HOOK_MATCH(HandleSwingFinish, &BeatmapObjectExecutionRatingsRecorder_CutScoreHandler::HandleSaberSwingRatingCounterDidFinish,
        void, BeatmapObjectExecutionRatingsRecorder_CutScoreHandler* self, ISaberSwingRatingCounter* counter) {
    
    HandleSwingFinish(self, counter);
    
    if(globalConfig.GetActive()) {
        auto itr = swingRatingMap.find(counter);
        if(itr == swingRatingMap.end()) {
            LOG_ERROR("Counter was not found in swingRatingMap!");
            return;
        }
        auto& noteCutInfo = itr->second.first;
        auto& flyingScoreEffect = itr->second.second;

        Judge(counter, flyingScoreEffect, noteCutInfo);

        swingRatingMap.erase(itr);

        if(flyingScoreEffect == currentEffect)
            currentEffect = nullptr;
    }
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

    try {
        throw std::runtime_error("const& error");
    } catch(std::runtime_error const& e) {
        LOG_INFO("error caught, %s", e.what());
    }
    
    try {
        throw std::runtime_error("non const& error");
    } catch(std::runtime_error e) {
        LOG_INFO("error caught, %s", e.what());
    }
    
    if(!direxists(ConfigsPath()))
        mkpath(ConfigsPath());

    try {
        if(!fileexists(GlobalConfigPath()))
            WriteToFile(GlobalConfigPath(), globalConfig);
        else
            ReadFromFile(GlobalConfigPath(), globalConfig);
    } catch(std::runtime_error const& e) {
        LOG_ERROR("Error loading config: %s", e.what());
    }
    LoadCurrentConfig();
	
    LOG_INFO("Completed setup!");
}

extern "C" void load() {
    il2cpp_functions::Init();

    QuestUI::Register::RegisterModSettingsViewController(modInfo, "Hit Score Visualizer", DidActivate);
    QuestUI::Register::RegisterMainMenuModSettingsViewController(modInfo, "Hit Score Visualizer", DidActivate);

    LOG_INFO("Installing hooks...");
    INSTALL_HOOK(getLogger(), InitFlyingScoreEffect);
    INSTALL_HOOK(getLogger(), HandleSwingChange);
    INSTALL_HOOK(getLogger(), HandleSwingFinish);
    INSTALL_HOOK(getLogger(), FlyingScoreEffectManualUpdate);
    LOG_INFO("Installed all hooks!");
}