#include "main.hpp"

#include "HitScoreFlowCoordinator.hpp"

#include "ViewControllers/ConfigViewController.hpp"
#include "ViewControllers/Judgment1ViewController.hpp"
#include "ViewControllers/Judgment2ViewController.hpp"
#include "ViewControllers/Judgment3ViewController.hpp"
#include "ViewControllers/Judgment4ViewController.hpp"
#include "ViewControllers/Judgment5ViewController.hpp"
#include "ViewControllers/Judgment6ViewController.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/QuestUI.hpp"

#include "HMUI/ViewController.hpp"
#include "HMUI/ViewController_AnimationType.hpp"
#include "HMUI/ViewController_AnimationDirection.hpp"
#include "HMUI/FlowCoordinator.hpp"
#include "PluginConfig.hpp"

using namespace HitScore;
using namespace HitScore::ViewControllers;

DEFINE_TYPE(HitScore, HitScoreFlowCoordinator);

int CurrentActive;

void HitScoreFlowCoordinator::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    CurrentActive = 1;
    if (firstActivation)
    {
        this->SetTitle(il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("HitScoreVisualizer Judgment 1"), HMUI::ViewController::AnimationDirection::Vertical);
        this->showBackButton = true;

        if(!this->HSConfigView) this->HSConfigView = QuestUI::BeatSaberUI::CreateViewController<ConfigViewController*>();
        if(!this->HSJudgment1View) this->HSJudgment1View = QuestUI::BeatSaberUI::CreateViewController<Judgment1ViewController*>();
        if(!this->HSJudgment2View) this->HSJudgment2View = QuestUI::BeatSaberUI::CreateViewController<Judgment2ViewController*>();
        if(!this->HSJudgment3View) this->HSJudgment3View = QuestUI::BeatSaberUI::CreateViewController<Judgment3ViewController*>();
        if(!this->HSJudgment4View) this->HSJudgment4View = QuestUI::BeatSaberUI::CreateViewController<Judgment4ViewController*>();
        if(!this->HSJudgment5View) this->HSJudgment5View = QuestUI::BeatSaberUI::CreateViewController<Judgment5ViewController*>();
        if(!this->HSJudgment6View) this->HSJudgment6View = QuestUI::BeatSaberUI::CreateViewController<Judgment6ViewController*>();

        HitScoreConfigViewController = reinterpret_cast<HitScore::ViewControllers::ConfigViewController*>(HSConfigView);

        std::function<void(int)> func = std::bind(&HitScoreFlowCoordinator::SubMenuButtonWasPressed, this, std::placeholders::_1);
        HitScoreConfigViewController->set_selectCallback(func);

        HitScoreFlowCoordinator::ProvideInitialViewControllers(HSJudgment1View, HitScoreConfigViewController, nullptr, nullptr, nullptr);
    }
}

void HitScoreFlowCoordinator::SubMenuButtonWasPressed(int VCtype) {
    if (!VCtype) return;
    //FindViewControllers(self);
    if (CurrentActive == VCtype) return;
    switch (VCtype)
    {
        case 1:
            if (!this->HSJudgment1View) this->HSJudgment1View = QuestUI::BeatSaberUI::CreateViewController<Judgment1ViewController*>();
            if (!this->HSJudgment1View) break;
            this->SetTitle(il2cpp_utils::createcsstr("HitScoreVisualizer Judgment 1"), HMUI::ViewController::AnimationType::In);
            this->ReplaceTopViewController(this->HSJudgment1View, this, this, nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Horizontal);
            CurrentActive = VCtype;
            break;
        case 3:
            if (!this->HSJudgment2View) this->HSJudgment2View = QuestUI::BeatSaberUI::CreateViewController<Judgment2ViewController*>();
            if (!this->HSJudgment2View) break;
            this->SetTitle(il2cpp_utils::createcsstr("HitScoreVisualizer Judgment 2"), HMUI::ViewController::AnimationType::In);
            this->ReplaceTopViewController(this->HSJudgment2View, this, this, nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Horizontal);
            CurrentActive = VCtype;
            break;
        case 2:
            if (!this->HSJudgment3View) this->HSJudgment3View = QuestUI::BeatSaberUI::CreateViewController<Judgment3ViewController*>();
            if (!this->HSJudgment3View || this->providedMainViewController == this->HSJudgment3View) break;
            this->SetTitle(il2cpp_utils::createcsstr("HitScoreVisualizer Judgment 3"), HMUI::ViewController::AnimationType::In);
            this->ReplaceTopViewController(this->HSJudgment3View, this, this, nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Horizontal);
            CurrentActive = VCtype;
            break;
        case 4:
            if (!this->HSJudgment4View) this->HSJudgment4View = QuestUI::BeatSaberUI::CreateViewController<Judgment4ViewController*>();
            if (!this->HSJudgment4View || this->providedMainViewController == this->HSJudgment4View) break;
            this->SetTitle(il2cpp_utils::createcsstr("HitScoreVisualizer Judgment 4"), HMUI::ViewController::AnimationType::In);
            this->ReplaceTopViewController(this->HSJudgment4View, this, this, nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Horizontal);
            CurrentActive = VCtype;
            break;
        case 5:
            if (!this->HSJudgment5View) this->HSJudgment5View = QuestUI::BeatSaberUI::CreateViewController<Judgment5ViewController*>();
            if (!this->HSJudgment5View || this->providedMainViewController == this->HSJudgment5View) break;
            this->SetTitle(il2cpp_utils::createcsstr("HitScoreVisualizer Judgment 5"), HMUI::ViewController::AnimationType::In);
            this->ReplaceTopViewController(this->HSJudgment5View, this, this, nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Horizontal);
            CurrentActive = VCtype;
            break;
        case 6:
            if (!this->HSJudgment6View) this->HSJudgment6View = QuestUI::BeatSaberUI::CreateViewController<Judgment6ViewController*>();
            if (!this->HSJudgment6View || this->providedMainViewController == this->HSJudgment6View) break;
            this->SetTitle(il2cpp_utils::createcsstr("HitScoreVisualizer Judgment 6"), HMUI::ViewController::AnimationType::In);
            this->ReplaceTopViewController(this->HSJudgment6View, this, this, nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Horizontal);
            CurrentActive = VCtype;
            break;
        default:
            getLogger().debug("switch default");
            break;
    }
}

void HitScoreFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topViewController) {
    getConfig().Write();

    HMUI::FlowCoordinator* ModSettingsFC = QuestUI::GetModSettingsFlowCoordinator();
    ModSettingsFC->DismissFlowCoordinator(this, HMUI::ViewController::AnimationDirection::Horizontal, nullptr, false);
}