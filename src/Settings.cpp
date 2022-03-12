#include "Main.hpp"
#include "Settings.hpp"

#include "HMUI/Touchable.hpp"
#include "HMUI/TableView_ScrollPositionType.hpp"

#include "questui/shared/BeatSaberUI.hpp"

#include <filesystem>

DEFINE_TYPE(HSV, CustomList);

using namespace QuestUI;
using namespace HSV;

std::vector<std::string> fullConfigPaths;
CustomList* configList = nullptr;
TMPro::TextMeshProUGUI* selectedConfig;

void ConfigSelected(int idx) {
    globalConfig.SelectedConfig = fullConfigPaths[idx];
    WriteToFile(GlobalConfigPath(), globalConfig);
    LoadCurrentConfig();
    selectedConfig->set_text("Current Config: " + configList->data[idx]);
}

void RefreshConfigList() {
    auto& data = configList->data;
    data.clear();
    fullConfigPaths.clear();
    int selectedIdx = -1;

    Config config;
    for(auto& entry : std::filesystem::recursive_directory_iterator(ConfigsPath())) {
        if (entry.path().extension() == ".json") {
            std::string fullPath = entry.path().string();
            // test loading the config
            try {
                ReadFromFile(fullPath, config);
            } catch(const std::exception& err) {
                LOG_ERROR("Could not load config file %s", fullPath.c_str());
                continue;
            }
            data.emplace_back(entry.path().stem().string());
            fullConfigPaths.emplace_back(fullPath);
            if(globalConfig.SelectedConfig == fullPath)
                selectedIdx = data.size() - 1;
        }
    }
    configList->tableView->ReloadData();
    if(selectedIdx >= 0) {
        configList->tableView->SelectCellWithIdx(selectedIdx, false);
        configList->tableView->ScrollToCellWithIdx(selectedIdx, HMUI::TableView::ScrollPositionType::Beginning, false);
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

        configList = BeatSaberUI::CreateScrollableCustomSourceList<CustomList*>(container, UnityEngine::Vector2(50, 60), ConfigSelected);
    }
    RefreshConfigList();
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
    }

    tableCell->set_text(data[idx]);
    return tableCell;
}

float CustomList::CellSize() {
    return cellSize;
}

int CustomList::NumberOfCells() {
    return data.size();
}