#pragma once

#include <map>

#include "GlobalNamespace/SimpleTextTableCell.hpp"
#include "HMUI/TableCell.hpp"
#include "HMUI/TableView.hpp"
#include "HMUI/ViewController.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "bsml/shared/BSML/Components/Settings/ToggleSetting.hpp"
#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(HSV, CustomList, UnityEngine::MonoBehaviour, classof(HMUI::TableView::IDataSource*),

    DECLARE_INSTANCE_FIELD(GlobalNamespace::SimpleTextTableCell*, simpleTextTableCellInstance);

    DECLARE_INSTANCE_FIELD(StringW, reuseIdentifier);
    DECLARE_INSTANCE_FIELD(float, cellSize);
    DECLARE_INSTANCE_FIELD(HMUI::TableView*, tableView);
    DECLARE_INSTANCE_FIELD(bool, expandCell);

    DECLARE_CTOR(ctor);

    DECLARE_OVERRIDE_METHOD_MATCH(HMUI::TableCell*, CellForIdx, &HMUI::TableView::IDataSource::CellForIdx, HMUI::TableView* tableView, int idx);
    DECLARE_OVERRIDE_METHOD_MATCH(float, CellSize, &HMUI::TableView::IDataSource::CellSize);
    DECLARE_OVERRIDE_METHOD_MATCH(int, NumberOfCells, &HMUI::TableView::IDataSource::NumberOfCells);

   public:
    std::vector<std::string> data;
    std::map<int, std::string> failures;
)

DECLARE_CLASS_CODEGEN(HSV, SettingsViewController, HMUI::ViewController,

    DECLARE_DEFAULT_CTOR();

    DECLARE_INSTANCE_FIELD(BSML::ToggleSetting*, enabledToggle);
    DECLARE_INSTANCE_FIELD(BSML::ToggleSetting*, hideToggle);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, selectedConfig);
    DECLARE_INSTANCE_FIELD(HSV::CustomList*, configList);

    DECLARE_INSTANCE_METHOD(void, ConfigSelected, int idx);
    DECLARE_INSTANCE_METHOD(void, RefreshConfigList);
    DECLARE_INSTANCE_METHOD(void, RefreshUI);

    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

   private:
    static std::vector<std::string> fullConfigPaths;
    static int selectedIdx;
)
