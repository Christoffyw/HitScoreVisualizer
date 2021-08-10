#include "PluginConfig.hpp"
#include "main.hpp"
#include "HsvFlyingScoreEffect.hpp"
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

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
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



MAKE_HOOK_MATCH(FlyingScoreEffectHook, &GlobalNamespace::EffectPoolsManualInstaller::ManualInstallBindings, void, GlobalNamespace::EffectPoolsManualInstaller* self, Zenject::DiContainer* diContainer, bool shortBeatEffect) {
    GameObject* gameObject = self->flyingScoreEffectPrefab->get_gameObject();

    FlyingScoreEffect* flyingScoreEffect = gameObject->GetComponent<FlyingScoreEffect*>();

    HitScore::HsvFlyingScoreEffect* hsvFlyingScoreEffect = gameObject->GetComponent<HitScore::HsvFlyingScoreEffect*>();

    if(gameObject != nullptr) {
        if(!hsvFlyingScoreEffect) {
            HitScore::HsvFlyingScoreEffect* hsvFlyingScoreEffect = gameObject->AddComponent<HitScore::HsvFlyingScoreEffect*>();

            FlyingObjectEffect* flyingObjectEffect = flyingScoreEffect;

            if(flyingScoreEffect != nullptr) {
                getLogger().info((flyingScoreEffect->moveAnimationCurve ? "move anim true": "false"));
                getLogger().info((flyingScoreEffect->shakeFrequency ? "shake freq true": "false"));
                getLogger().info((flyingScoreEffect->shakeStrength ? "shake streng true": "false"));
                getLogger().info((flyingScoreEffect->shakeStrengthAnimationCurve ? "shake anim true": "false"));
                getLogger().info((flyingScoreEffect->fadeAnimationCurve ? "fade anim true": "false"));
                getLogger().info((flyingScoreEffect->text ? "text true": "false"));
                getLogger().info((flyingScoreEffect->maxCutDistanceScoreIndicator ? "max cut true": "false"));

                getLogger().info((hsvFlyingScoreEffect->moveAnimationCurve ? "move anim HSV": "false"));
                getLogger().info((hsvFlyingScoreEffect->shakeFrequency ? "shake freq HSV": "false"));
                getLogger().info((hsvFlyingScoreEffect->shakeStrength ? "shake streng HSV": "false"));
                getLogger().info((hsvFlyingScoreEffect->shakeStrengthAnimationCurve ? "shake anim HSV": "false"));
                getLogger().info((hsvFlyingScoreEffect->fadeAnimationCurve ? "fade anim HSV": "false"));
                getLogger().info((hsvFlyingScoreEffect->text ? "text HSV": "false"));
                getLogger().info((hsvFlyingScoreEffect->maxCutDistanceScoreIndicator ? "max cut HSV": "false"));

                hsvFlyingScoreEffect->moveAnimationCurve = flyingScoreEffect->moveAnimationCurve;
                hsvFlyingScoreEffect->shakeFrequency = flyingScoreEffect->shakeFrequency;
                hsvFlyingScoreEffect->shakeStrength = flyingScoreEffect->shakeStrength;
                hsvFlyingScoreEffect->shakeStrengthAnimationCurve = flyingScoreEffect->shakeStrengthAnimationCurve;
                hsvFlyingScoreEffect->fadeAnimationCurve = flyingScoreEffect->fadeAnimationCurve;
                hsvFlyingScoreEffect->text = flyingScoreEffect->text;
                hsvFlyingScoreEffect->maxCutDistanceScoreIndicator = flyingScoreEffect->maxCutDistanceScoreIndicator;

                HitScore::JudgmentService* judgmentService = gameObject->AddComponent<HitScore::JudgmentService*>();

                hsvFlyingScoreEffect->Construct(judgmentService);

                /*if(flyingScoreEffect->moveAnimationCurve != nullptr)
                    hsvFlyingScoreEffect->_set__moveAnimationCurve(flyingObjectEffect->moveAnimationCurve);
                if(flyingScoreEffect->shakeFrequency)
                    hsvFlyingScoreEffect->_set__shakeFrequency(flyingObjectEffect->shakeFrequency);
                if(flyingScoreEffect->shakeStrength)
                    hsvFlyingScoreEffect->_set__shakeStrength(flyingObjectEffect->shakeStrength);
                if(flyingScoreEffect->shakeStrengthAnimationCurve != nullptr)
                    hsvFlyingScoreEffect->_set__shakeStrengthAnimationCurve(flyingObjectEffect->shakeStrengthAnimationCurve);

                if(flyingScoreEffect->text != nullptr)
                    hsvFlyingScoreEffect->_set__text(flyingScoreEffect->text);
                if(flyingScoreEffect->fadeAnimationCurve != nullptr)
                    hsvFlyingScoreEffect->_set__fadeAnimationCurve(flyingScoreEffect->fadeAnimationCurve);
                if(flyingScoreEffect->maxCutDistanceScoreIndicator != nullptr)
                    hsvFlyingScoreEffect->_set__maxCutDistanceScoreIndicator(flyingScoreEffect->maxCutDistanceScoreIndicator);
                    */
            }
        }
    }

    FlyingScoreEffectHook(self, diContainer, shortBeatEffect);
}

void HitScore::Main::Update() {

}

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    getLogger().info("DidActivate: %p, %d, %d, %d", self, firstActivation, addedToHierarchy, screenSystemEnabling);


    /*
    if(firstActivation) {
        self->get_gameObject()->AddComponent<HMUI::Touchable*>();
        self->get_gameObject()->AddComponent<HitScore::Main*>();
        GameObject* container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());

        auto* textGrid = container;
        //        textGrid->set_spacing(1);

        QuestUI::BeatSaberUI::CreateText(textGrid->get_transform(), "Faster Scroll Mod settings.");
        QuestUI::BeatSaberUI::AddHoverHint(AddConfigValueToggle(textGrid->get_transform(), getPluginConfig().IsStock)->get_gameObject(),"Toggles whether the mod is active or not.");

        //        buttonsGrid->set_spacing(1);

        auto* floatGrid = container;

        QuestUI::BeatSaberUI::AddHoverHint(AddConfigValueIncrementFloat(floatGrid->get_transform(), getPluginConfig().MaxSpeed, 0, 10, 30, 1000)->get_gameObject(),"Changes the speed of scrolling with the joystick. Default: 600");
        QuestUI::BeatSaberUI::AddHoverHint(AddConfigValueIncrementFloat(floatGrid->get_transform(), getPluginConfig().Accel, 1, 0.1, 0.5, 5)->get_gameObject(),"Changes the acceleration speed of scrolling with the joystick. Default: 1.5");
        QuestUI::BeatSaberUI::AddHoverHint(AddConfigValueToggle(floatGrid->get_transform(), getPluginConfig().IsLinear)->get_gameObject(),"Toggles whether you want to use acceleration or not.");
    }
     */
}

extern "C" void load() {
  il2cpp_functions::Init();
  QuestUI::Init();
  QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);
  HitScore::InstallHooks();
  INSTALL_HOOK(getLogger(), FlyingScoreEffectHook);
  custom_types::Register::AutoRegister();
}