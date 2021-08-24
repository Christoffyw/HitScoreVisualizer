#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/FlowCoordinator.hpp"
#include "HMUI/ViewController.hpp"

#include "ViewControllers/ConfigViewController.hpp"

DECLARE_CLASS_CODEGEN(HitScore, HitScoreFlowCoordinator, HMUI::FlowCoordinator,
    DECLARE_INSTANCE_FIELD_DEFAULT(HitScore::ViewControllers::ConfigViewController*, HitScoreConfigViewController, nullptr);
    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::FindMethodUnsafe("HMUI", "FlowCoordinator", "DidActivate", 3), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD(void, BackButtonWasPressed, il2cpp_utils::FindMethodUnsafe("HMUI", "FlowCoordinator", "BackButtonWasPressed", 1), HMUI::ViewController* topViewController);

    DECLARE_INSTANCE_FIELD(HMUI::ViewController*, HSConfigView);
    DECLARE_INSTANCE_FIELD(HMUI::ViewController*, HSJudgment1View);
    DECLARE_INSTANCE_FIELD(HMUI::ViewController*, HSJudgment2View);
    DECLARE_INSTANCE_FIELD(HMUI::ViewController*, HSJudgment3View);
    DECLARE_INSTANCE_FIELD(HMUI::ViewController*, HSJudgment4View);
    DECLARE_INSTANCE_FIELD(HMUI::ViewController*, HSJudgment5View);
    DECLARE_INSTANCE_FIELD(HMUI::ViewController*, HSJudgment6View);

    public:
        void SubMenuButtonWasPressed(int VCtype);
);



