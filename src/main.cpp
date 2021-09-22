#include "PluginConfig.hpp"
#include "main.hpp"

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

#include "GlobalNamespace/FlyingScoreEffect_Pool.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounter.hpp"
#include "GlobalNamespace/ScoreModel.hpp"
#include "GlobalNamespace/FlyingScoreSpawner.hpp"
#include "GlobalNamespace/BeatmapObjectExecutionRatingsRecorder_CutScoreHandler.hpp"
#include "GlobalNamespace/ColorExtensions.hpp"


#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/register.hpp"
#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"
#include "Zenject/DiContainer.hpp"
#include <iostream>
#include <string>
#include <utility>
#include <iomanip>
#include <sstream>
#include <regex>
#include <filesystem>

namespace fs = std::filesystem;

using namespace std;
using namespace HMUI;
using namespace GlobalNamespace;
using namespace QuestUI;
using namespace UnityEngine;

HSVConfig HSV::config;
bool HSV::configValid = false;
std::string currentConfig;

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

#define HANDLE_CONFIG_FAILURE(logger, condition) do { \
    if (!condition) { \
        logger.error("Config failed to load properly!"); \
        configValid = false; \
    } else { \
        configValid = true; \
    } \
} while(0)

const ModInfo& getModInfo() {
    return modInfo;
}

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

void makeFolder() {
    if (!direxists(getDataDir(modInfo)))
    {
        int makePath = mkpath(getDataDir(modInfo));
        if (makePath == -1)
        {
            getLogger().error("Failed to make configs folder path!");
        }
    }
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;

    getPluginConfig().Init(info);

    getLogger().info("Completed setup!");
}

// used for fixed pos
FlyingScoreEffect* currentEffect = nullptr;
std::map<GlobalNamespace::ISaberSwingRatingCounter*, GlobalNamespace::NoteCutInfo> swingRatingMap;

void Judge(FlyingScoreEffect* self, ISaberSwingRatingCounter* counter, bool isFinish) {

    auto itr = swingRatingMap.find(counter);
    if(itr == swingRatingMap.end()) {
        getLogger().info("counter was not found in swingRatingMap!");
        return;
    }
    auto noteCutInfo = itr->second;

    int before;
    int after;
    int accuracy;

    ScoreModel::RawScoreWithoutMultiplier(counter, noteCutInfo.cutDistanceToCenter, byref(before), byref(after), byref(accuracy));
    int total = before + after + accuracy;
    float timeDependence = std::abs(noteCutInfo.cutNormal.z);
    HSV::checkJudgments(self, total, before, after, accuracy, timeDependence);
    if(isFinish)
        swingRatingMap.erase(itr);
}

MAKE_HOOK_MATCH(InitFlyingScoreEffect, &FlyingScoreEffect::InitAndPresent, void, FlyingScoreEffect* self, ByRef<NoteCutInfo> noteCutInfo, int multiplier, float duration, Vector3 targetPos, Quaternion rotation, Color color) {
    if(getPluginConfig().IsEnabled.GetValue()) {
        if(HSV::config.useFixedPos) {

            targetPos.x = HSV::config.fixedPosX;
            targetPos.y = HSV::config.fixedPosY;
            targetPos.z = HSV::config.fixedPosZ;

            auto transform = self->get_transform();
            transform->set_position(targetPos);

            if(currentEffect) {
                //currentEffect->duration = 0;
                getLogger().info("remove effect");
            }
            currentEffect = self;

            getLogger().info("new current effect set");
        }
    }

    InitFlyingScoreEffect(self, noteCutInfo, multiplier, duration, targetPos, rotation, color);

    if(getPluginConfig().IsEnabled.GetValue()) {
        //duration = 0.01;
        self->text->set_text(il2cpp_utils::newcsstr(""));
        self->maxCutDistanceScoreIndicator->set_enabled(false);

        // store note cut info for time dependence
        swingRatingMap.insert(std::make_pair(self->saberSwingRatingCounter, noteCutInfo.heldRef));
    }
}

MAKE_HOOK_MATCH(HandleSwingChange, &FlyingScoreEffect::HandleSaberSwingRatingCounterDidChange, void, FlyingScoreEffect* self, ISaberSwingRatingCounter* counter, float rating) {
    HandleSwingChange(self, counter, rating);
    Judge(self, counter, false);
}

MAKE_HOOK_MATCH(HandleSwingFinish, &FlyingScoreEffect::HandleSaberSwingRatingCounterDidFinish, void, FlyingScoreEffect* self, ISaberSwingRatingCounter* counter) {
    HandleSwingFinish(self, counter);
    Judge(self, counter, true);
}

MAKE_HOOK_MATCH(FlyingScoreEffectFinish, &GlobalNamespace::FlyingScoreSpawner::HandleFlyingObjectEffectDidFinish, void, FlyingScoreSpawner* self, FlyingObjectEffect* flyingObjectEffect) {
    FlyingScoreEffectFinish(self, flyingObjectEffect);
    if(flyingObjectEffect == currentEffect) {
        currentEffect = nullptr;
        getLogger().info("CLEAR CURRENT EFFECT FOR NEXT ONE");
    }
}

MAKE_HOOK_MATCH(FlyingScoreEffectManualUpdate, &GlobalNamespace::FlyingScoreEffect::ManualUpdate, void, FlyingScoreEffect* self, float t) {
    FlyingScoreEffectManualUpdate(self, t);
    Color color = ColorExtensions::ColorWithAlpha(self->color, self->fadeAnimationCurve->Evaluate(t) * self->colorAMultiplier);
    self->text->set_color(color);
}

void HSV::loadConfig() {
    static auto logger = getLogger().WithContext("HSV").WithContext("loadConfig");
    logger.info("Loading Configuration...");

    getConfig().Reload();

    auto json = readfile(getDataDir(modInfo) + currentConfig);
    ConfigDocument doc;
    doc.Parse(json.c_str());

    logger.info("%s",currentConfig.c_str());

    HANDLE_CONFIG_FAILURE(logger, ConfigHelper::LoadConfig(config, doc));
    if (!configValid && config.isDefaultConfig) {
        logger.debug("Setting to default because config failed to load, even though it was default config!");
        config.SetToDefault();
        config.WriteToConfig(doc);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);
        writefile(getDataDir(modInfo) + currentConfig, buffer.GetString());

        configValid = true;
    }
    if (configValid) {
        logger.info("Loaded Configuration!");
    } else {
        logger.info("Configuration is invalid! Please ensure the config is the correct format!");
    }
}

GameObject* scrollView;
std::list<UnityEngine::UI::Button*> configList = {};
TMPro::TextMeshProUGUI* displaySelectedText;

void SelectButtonConfig() {
    for (UnityEngine::UI::Button* button : configList)
    {
        if (button->hasSelection)
        {
            std::string filename = to_utf8(csstrtostr(button->GetComponentInChildren<TMPro::TextMeshProUGUI*>()->get_text()));
            getPluginConfig().SelectedConfig.SetValue(filename);
            currentConfig = filename;
            HSV::loadConfig();
            getLogger().debug("Selected filename %s", filename.c_str());
            displaySelectedText->set_text(il2cpp_utils::newcsstr("Current Config: " + getPluginConfig().SelectedConfig.GetValue()));
        }
    }
}

void RefreshConfigList()
{
    for (UnityEngine::UI::Button* button : configList) UnityEngine::Object::Destroy(button->get_transform()->get_parent()->get_gameObject());
    configList = {};

    for(const auto & entry : fs::directory_iterator(getDataDir(modInfo).c_str())) {
        std::string filename = entry.path().string();
        while (filename.find(getDataDir(modInfo).c_str()) != string::npos)
            filename.replace(filename.find(getDataDir(modInfo).c_str()), 64, "");
        if (std::regex_search(filename, std::regex(".json")))
        {
            UnityEngine::UI::HorizontalLayoutGroup* rowgroup = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(scrollView->get_transform());
            UnityEngine::UI::Button* button = QuestUI::BeatSaberUI::CreateUIButton(rowgroup->get_rectTransform(), filename, SelectButtonConfig);
            button->get_gameObject()->GetComponentInChildren<TMPro::TextMeshProUGUI*>()->set_fontStyle(2);
            configList.push_back(button);
        }
    }
}

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    getLogger().info("DidActivate: %p, %d, %d, %d", self, firstActivation, addedToHierarchy, screenSystemEnabling);

    if(firstActivation) {
        self->get_gameObject()->AddComponent<HMUI::Touchable*>();
        UnityEngine::UI::VerticalLayoutGroup* container = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(self->get_transform());

        auto textGrid = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(container->get_rectTransform());
        textGrid->set_childAlignment(UnityEngine::TextAnchor::UpperCenter);
        textGrid->set_childForceExpandHeight(false);
        textGrid->set_childControlHeight(true);
        //        textGrid->set_spacing(1);

        QuestUI::BeatSaberUI::CreateText(textGrid->get_rectTransform(), "HitScoreVisualizer settings.");
        QuestUI::BeatSaberUI::AddHoverHint(AddConfigValueToggle(textGrid->get_rectTransform(), getPluginConfig().IsEnabled)->get_gameObject(),"Toggles whether the mod is active or not.");

        QuestUI::BeatSaberUI::CreateText(textGrid->get_rectTransform(), "<br><br>");
        displaySelectedText = QuestUI::BeatSaberUI::CreateText(textGrid->get_rectTransform(), "Current Config: " + getPluginConfig().SelectedConfig.GetValue());

        auto configListScroll = QuestUI::BeatSaberUI::CreateScrollView(container->get_rectTransform());
        configListScroll->GetComponent<QuestUI::ExternalComponents*>()->Get<UnityEngine::UI::LayoutElement*>()->set_minHeight(56.0);

        scrollView = configListScroll;

        auto configListContainer = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(configListScroll->get_transform());
        configListContainer->set_childAlignment(UnityEngine::TextAnchor::UpperCenter);
        configListContainer->set_childForceExpandHeight(false);
        configListContainer->set_childControlHeight(true);

        configListScroll->get_gameObject()->SetActive(getPluginConfig().IsEnabled.GetValue());

        RefreshConfigList();
        //        buttonsGrid->set_spacing(1);

        //QuestUI::BeatSaberUI::AddHoverHint(AddConfigValueIncrementFloat(textGrid->get_transform(), getPluginConfig().MaxSpeed, 0, 10, 30, 1000)->get_gameObject(),"Changes the speed of scrolling with the joystick. Default: 600");
        //QuestUI::BeatSaberUI::AddHoverHint(AddConfigValueIncrementFloat(textGrid->get_transform(), getPluginConfig().Accel, 1, 0.1, 0.5, 5)->get_gameObject(),"Changes the acceleration speed of scrolling with the joystick. Default: 1.5");
        //QuestUI::BeatSaberUI::AddHoverHint(AddConfigValueToggle(textGrid->get_transform(), getPluginConfig().IsLinear)->get_gameObject(),"Toggles whether you want to use acceleration or not.");
    }

}

std::optional<int> HSV::getBestJudgment(std::vector<Judgment> &judgments, int comparison) {
    static auto length = config.judgments.size();
    if (length == 0) {
        return std::nullopt;
    }
    int i;
    for (i = 0; i < length; i++) {
        if (comparison >= config.judgments[i].threshold) {
            break;
        }
    }
    return i;
}

std::optional<const Segment> HSV::getBestSegment(std::vector<Segment>& segments, int comparison) {
    auto size = segments.size();
    if (size == 0) {
        return std::nullopt;
    }
    int bestIdx = 0;
    for (int i = 1; i < size; i++) {
        if (comparison >= segments[bestIdx].threshold) {
            break;
        }
        bestIdx = i;
    }
    return segments[bestIdx];
}

std::optional<const TimeSegment> HSV::getBestTimeSegment(std::vector<TimeSegment>& segments, int comparison) {
    auto size = segments.size();
    if (size == 0) {
        return std::nullopt;
    }
    int bestIdx = 0;
    for (int i = 1; i < size; i++) {
        if (comparison >= segments[bestIdx].threshold) {
            break;
        }
        bestIdx = i;
    }
    return segments[bestIdx];
}

void HSV::checkJudgments(GlobalNamespace::FlyingScoreEffect* flyingScoreEffect, int total, int before, int after, int accuracy, float timeDependence) {
    if(!configValid)
        return;
    auto bestIndex = getBestJudgment(config.judgments, total);
    auto best = config.judgments[bestIndex.value()];

    flyingScoreEffect->text->set_text(il2cpp_utils::newcsstr(JudgeText(total, before, after, accuracy, timeDependence, bestIndex.value())));
    flyingScoreEffect->text->set_color(JudgeColor(total, before, after, accuracy, timeDependence, bestIndex.value()));
    flyingScoreEffect->text->set_richText(true);
    flyingScoreEffect->text->set_enableWordWrapping(false);
    flyingScoreEffect->text->set_overflowMode(TMPro::TextOverflowModes::Overflow);
    flyingScoreEffect->color = JudgeColor(total, before, after, accuracy, timeDependence, bestIndex.value());
}

std::string HSV::JudgeText(int score, int before, int after, int accuracy, float timeDependence, int bestIndex) {
    return DisplayModeFormat(score, before, after, accuracy, timeDependence, config.judgments[bestIndex]);
}

UnityEngine::Color HSV::JudgeColor(int score, int before, int after, int accuracy, float timeDependence, int bestIndex) {
    return config.judgments[bestIndex].color.value();
}

std::string HSV::JudgeSegment(int scoreForSegment, std::vector<Segment> segments) {
    if(segments.size() == 0) {
        return std::string();
    }

    Segment bestSegment = getBestSegment(segments, scoreForSegment).value();

    if(bestSegment.text.value().empty())
        return std::string();
    else
        return bestSegment.text.value();
}

std::string HSV::JudgeTimeSegment(int scoreForSegment, std::vector<TimeSegment> judgments) {
    if(judgments.size() == 0) {
        return std::string();
    }

    TimeSegment bestSegment = getBestTimeSegment(judgments, scoreForSegment).value();

    if(bestSegment.text.value().empty())
        return std::string();
    else
        return bestSegment.text.value();
}

std::string HSV::ConvertTimeDependencePrecision(float timeDependence, int decimalOffset, int decimalPrecision) {
    float multiplier = std::pow(10, decimalOffset);
    std::stringstream stream;
    stream << std::fixed << std::setprecision(decimalPrecision) << (timeDependence * multiplier);
    std::string s = stream.str();
    return s;
}

std::string HSV::DisplayModeFormat(int score, int before, int after, int accuracy, float timeDependence, const Judgment& judgment) {
    auto text = judgment.text.value();
    text.set_beforeCut(std::to_string(before));
    text.set_accuracy(std::to_string(accuracy));
    text.set_afterCut(std::to_string(after));
    text.set_score(std::to_string(score));
    text.set_percent(string_format("%f", std::round(score / 115 * 100)));
    text.set_timeDependency(ConvertTimeDependencePrecision(timeDependence, config.timeDependencyDecimalOffset, config.timeDependencyDecimalPrecision));
    text.set_beforeCutSegment(JudgeSegment(before, config.beforeCutAngleJudgments));
    text.set_accuracySegment(JudgeSegment(accuracy, config.accuracyJudgments));
    text.set_afterCutSegment(JudgeSegment(after, config.afterCutAngleJudgments));
    text.set_timeDependencySegment(JudgeTimeSegment(timeDependence, config.timeDependencyJudgments));

    std::string s = text.Join();
    // getLogger().debug("Calculated score %s", s.c_str());
    return s;
}

extern "C" void load() {
    custom_types::Register::AutoRegister();
    il2cpp_functions::Init();
    currentConfig = getPluginConfig().SelectedConfig.GetValue();
    makeFolder();
    QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);
    QuestUI::Register::RegisterMainMenuModSettingsViewController(modInfo, DidActivate);
    HSV::loadConfig();
    INSTALL_HOOK(getLogger(), InitFlyingScoreEffect);
    INSTALL_HOOK(getLogger(), HandleSwingChange);
    INSTALL_HOOK(getLogger(), HandleSwingFinish);
    INSTALL_HOOK(getLogger(), FlyingScoreEffectFinish);
    INSTALL_HOOK(getLogger(), FlyingScoreEffectManualUpdate);
}
