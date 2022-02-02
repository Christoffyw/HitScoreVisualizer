#include "Main.hpp"

#include "HMUI/Touchable.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"

#include <filesystem>

using namespace QuestUI;

std::vector<UnityEngine::UI::Button*> buttonList;
UnityEngine::GameObject* scrollView = nullptr;
TMPro::TextMeshProUGUI* selectedConfig;

void ButtonSelected() {
    for(auto& button : buttonList)  {
        if(button->hasSelection) {
            std::string filename = button->GetComponentInChildren<TMPro::TextMeshProUGUI*>()->get_text();
            globalConfig.SelectedConfig = filename + ".json";
            WriteToFile(GlobalConfigPath(), globalConfig);
            LoadCurrentConfig();
            selectedConfig->set_text("Current Config: " + filename);
        }
    }
}

void RefreshConfigList() {
    // should use an aactual list but I'm too lazy for now
    for(auto& button : buttonList) {
        UnityEngine::Object::Destroy(button->get_transform()->get_parent()->get_gameObject());
    }
    buttonList.clear();

    for(auto& entry : std::filesystem::recursive_directory_iterator(ConfigsPath())) {
        if (entry.path().extension() == ".json") {
            UnityEngine::UI::Button* button = BeatSaberUI::CreateUIButton(scrollView->get_transform(), entry.path().stem().string(), ButtonSelected);
            button->GetComponentInChildren<TMPro::TextMeshProUGUI*>()->set_fontStyle(2);
            buttonList.push_back(button);
        }
    }
}

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {

    if(firstActivation) {
        self->get_gameObject()->AddComponent<HMUI::Touchable*>();
        auto container = BeatSaberUI::CreateVerticalLayoutGroup(self->get_transform());

        auto textLayout = BeatSaberUI::CreateVerticalLayoutGroup(container->get_transform());
        textLayout->set_childForceExpandHeight(false);
        textLayout->set_childForceExpandWidth(false);
        textLayout->set_childControlHeight(true);
        textLayout->set_spacing(1.5);

        BeatSaberUI::CreateText(textLayout->get_transform(), "HitScoreVisualizer settings.");
        BeatSaberUI::AddHoverHint(BeatSaberUI::CreateToggle(textLayout->get_transform(), "Mod Enabled", globalConfig.ModEnabled, [](bool enabled){
            globalConfig.ModEnabled = enabled;
            WriteToFile(GlobalConfigPath(), globalConfig);
        })->get_gameObject(), "Toggles whether the mod is active or not");

        selectedConfig = BeatSaberUI::CreateText(textLayout->get_transform(), "Current Config: " + std::filesystem::path(globalConfig.SelectedConfig).stem().string(), false);

        scrollView = BeatSaberUI::CreateScrollView(container->get_transform());
        scrollView->GetComponent<ExternalComponents*>()->Get<UnityEngine::UI::LayoutElement*>()->set_minHeight(56);
    }
    RefreshConfigList();
}