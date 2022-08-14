#pragma once

#include "GlobalNamespace/SimpleTextTableCell.hpp"

#include "UnityEngine/UI/Toggle.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "HMUI/ViewController.hpp"
#include "HMUI/TableCell.hpp"
#include "HMUI/TableView_IDataSource.hpp"

#include "custom-types/shared/macros.hpp"

#include <map>

___DECLARE_TYPE_WRAPPER_INHERITANCE(HSV, CustomList, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, UnityEngine::MonoBehaviour, "HSV", { classof(HMUI::TableView::IDataSource*) }, 0, nullptr,

    DECLARE_INSTANCE_FIELD(GlobalNamespace::SimpleTextTableCell*, simpleTextTableCellInstance);

    DECLARE_INSTANCE_FIELD(StringW, reuseIdentifier);
    DECLARE_INSTANCE_FIELD(float, cellSize);
    DECLARE_INSTANCE_FIELD(HMUI::TableView*, tableView);
    DECLARE_INSTANCE_FIELD(bool, expandCell);

    DECLARE_CTOR(ctor);
    
    DECLARE_OVERRIDE_METHOD(HMUI::TableCell*, CellForIdx, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::TableView::IDataSource::CellForIdx>::get(), HMUI::TableView* tableView, int idx);
    DECLARE_OVERRIDE_METHOD(float, CellSize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::TableView::IDataSource::CellSize>::get());
    DECLARE_OVERRIDE_METHOD(int, NumberOfCells, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::TableView::IDataSource::NumberOfCells>::get());

    public:
    std::vector<std::string> data;
    std::map<int, std::string> failures;
)

DECLARE_CLASS_CODEGEN(HSV, SettingsViewController, HMUI::ViewController,

    UnityEngine::UI::Toggle* enabledToggle;
    TMPro::TextMeshProUGUI* selectedConfig;
    HSV::CustomList* configList;
    
    static std::vector<std::string> fullConfigPaths;
    static int selectedIdx;

    void ConfigSelected(int idx);
    void RefreshConfigList();
    void RefreshUI();

    DECLARE_DEFAULT_CTOR();

    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::ViewController::DidActivate>::get(), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
)