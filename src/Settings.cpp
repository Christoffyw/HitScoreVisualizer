#include "Main.hpp"
#include "Settings.hpp"
#include "json/DefaultConfig.hpp"

#include "HMUI/Touchable.hpp"
#include "HMUI/TableView_ScrollPositionType.hpp"

#include "questui/shared/BeatSaberUI.hpp"

#include <filesystem>

DEFINE_TYPE(HSV, CustomList);
DEFINE_TYPE(HSV, SettingsViewController);

using namespace QuestUI;
using namespace HSV;

int SettingsViewController::selectedIdx = -1;
std::vector<std::string> SettingsViewController::fullConfigPaths = {};

void SettingsViewController::ConfigSelected(int idx) {
    selectedIdx = idx;
    globalConfig.SelectedConfig = fullConfigPaths[idx];
    WriteToFile(GlobalConfigPath(), globalConfig);
    LoadCurrentConfig();
    selectedConfig->set_text("Current Config: " + configList->data[idx]);
}

void SettingsViewController::RefreshConfigList() {
    auto& data = configList->data;
    auto& failureMap = configList->failures;
    data.clear();
    failureMap.clear();
    fullConfigPaths.clear();

    Config config;
    for(auto& entry : std::filesystem::recursive_directory_iterator(ConfigsPath())) {
        std::string fullPath = entry.path().string();
        bool retry = false;
        std::string failed = "";
        // test loading the config
        do {
            try {
                ReadFromFile(fullPath, config);
            } catch(const std::exception& err) {
                LOG_ERROR("Could not load config file %s: %s", fullPath.c_str(), err.what());
                if(config.IsDefault) {
                    writefile(fullPath, defaultConfigText);
                    retry = !retry;
                } else
                    failed = std::string("Error loading config: ") + err.what();
            }
        } while(retry);
        if(!failed.empty()) {
            std::string redPath = "<color=red>" + entry.path().stem().string();
            data.emplace_back(redPath);
            fullConfigPaths.emplace_back(fullPath);
            failureMap.insert({data.size() - 1, failed});
            continue;
        }
        data.emplace_back(entry.path().stem().string());
        fullConfigPaths.emplace_back(fullPath);
        if(globalConfig.SelectedConfig == fullPath)
            selectedIdx = data.size() - 1;
    }
    configList->tableView->ReloadData();
    if(selectedIdx >= 0) {
        configList->tableView->SelectCellWithIdx(selectedIdx, false);
        configList->tableView->ScrollToCellWithIdx(selectedIdx, HMUI::TableView::ScrollPositionType::Beginning, false);
    }
}

void SettingsViewController::RefreshUI() {
    RefreshConfigList();
    selectedConfig->set_text("Current Config: " + configList->data[selectedIdx]);
    enabledToggle->set_isOn(globalConfig.ModEnabled);
    hideToggle->set_isOn(globalConfig.HideUntilDone);
}

void SettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {

    if(firstActivation) {
        get_gameObject()->AddComponent<HMUI::Touchable*>();
        auto container = BeatSaberUI::CreateVerticalLayoutGroup(get_transform());

        auto textLayout = BeatSaberUI::CreateVerticalLayoutGroup(container->get_transform());
        textLayout->set_childForceExpandHeight(false);
        textLayout->set_childForceExpandWidth(false);
        textLayout->set_childControlHeight(true);
        textLayout->set_spacing(1.5);

        BeatSaberUI::CreateText(textLayout, "HitScoreVisualizer settings.");
        enabledToggle = BeatSaberUI::CreateToggle(textLayout->get_transform(), "Mod Enabled", globalConfig.ModEnabled, [](bool enabled){
            globalConfig.ModEnabled = enabled;
            WriteToFile(GlobalConfigPath(), globalConfig);
        });
        BeatSaberUI::AddHoverHint(enabledToggle, "Toggles whether the mod is active or not");

        hideToggle = BeatSaberUI::CreateToggle(textLayout->get_transform(), "Hide Until Calculation Finishes", globalConfig.HideUntilDone, [](bool enabled){
            globalConfig.HideUntilDone = enabled;
            WriteToFile(GlobalConfigPath(), globalConfig);
        });
        BeatSaberUI::AddHoverHint(enabledToggle, "With this enabled, the hit scores will not be displayed until the score has been finalized");

        selectedConfig = BeatSaberUI::CreateText(textLayout, "Current Config: " + std::filesystem::path(globalConfig.SelectedConfig).stem().string(), false);

        configList = BeatSaberUI::CreateScrollableCustomSourceList<CustomList*>(container, UnityEngine::Vector2(50, 50), [this](int idx) { ConfigSelected(idx); });
    }
    RefreshUI();
}

void CustomList::ctor() {
    INVOKE_CTOR();
    expandCell = false;
    reuseIdentifier = StringW("HSVConfigListTableCell");
    cellSize = 8;
    tableView = nullptr;
}

HMUI::TableCell* CustomList::CellForIdx(HMUI::TableView* tableView, int idx) {
    auto tableCell = (GlobalNamespace::SimpleTextTableCell*) tableView->DequeueReusableCellForIdentifier(reuseIdentifier);
    if (!tableCell) {
        auto simpleTextTableCellInstance = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::SimpleTextTableCell*>().First([](auto x) {
            return x->get_name() == "SimpleTextTableCell";
        });
        tableCell = Instantiate(simpleTextTableCellInstance);
        tableCell->set_reuseIdentifier(reuseIdentifier);

        tableCell->text->set_richText(true);
        tableCell->text->set_enableWordWrapping(false);
        BeatSaberUI::AddHoverHint(tableCell, "");
    }

    tableCell->set_text(data[idx]);
    tableCell->GetComponent<HMUI::HoverHint*>()->set_text("");
    tableCell->set_interactable(true);
    if(failures.contains(idx)) {
        tableCell->GetComponent<HMUI::HoverHint*>()->set_text(failures[idx]);
        tableCell->set_interactable(false);
    }
    return tableCell;
}

float CustomList::CellSize() {
    return cellSize;
}

int CustomList::NumberOfCells() {
    return data.size();
}
