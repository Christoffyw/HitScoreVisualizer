#include "ViewControllers/ConfigViewController.hpp"
#include "main.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/QuestUI.hpp"

#include "UnityEngine/RectOffset.hpp"

#include "UnityEngine/Object.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/Events/UnityAction_1.hpp"
#include "UnityEngine/GameObject.hpp"
#include "HMUI/Touchable.hpp"
#include "HMUI/ScrollView.hpp"
using namespace QuestUI;
using namespace UnityEngine::UI;
using namespace UnityEngine;
using namespace HMUI;

DEFINE_TYPE(HitScore::ViewControllers, ConfigViewController);

namespace HitScore::ViewControllers {
    void ConfigViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if(firstActivation) {
            get_gameObject()->AddComponent<Touchable*>();

            GameObject* mainLayout = GameObject::New_ctor();
            RectTransform* parent = mainLayout->AddComponent<RectTransform*>();
            parent->SetParent(get_transform(), false);
            parent->set_localPosition(UnityEngine::Vector3(38.0f, 0.0f));

            VerticalLayoutGroup* settingsLayout = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(parent);
            RectTransform* settingsLayoutTransform = settingsLayout->GetComponent<RectTransform*>();
            settingsLayout->get_gameObject()->AddComponent<Backgroundable*>()->ApplyBackground(il2cpp_utils::newcsstr("round-rect-panel"));
            settingsLayout->set_spacing(1.2f);
            settingsLayout->set_padding(UnityEngine::RectOffset::New_ctor(3, 3, 2, 2));

            ContentSizeFitter* contentSizeFitter = settingsLayout->get_gameObject()->AddComponent<ContentSizeFitter*>();
            contentSizeFitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
            contentSizeFitter->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);

            BeatSaberUI::CreateUIButton(settingsLayoutTransform, "Judgment 1",
                                        [&]() {
                getLogger().debug("Judgment 1 Button pressed!");
                this->callback(1);
            })->get_transform()->GetParent()->GetComponent<LayoutElement*>()->set_preferredWidth(50.0f);

            BeatSaberUI::CreateUIButton(settingsLayoutTransform, "Judgment 2",
                                        [&]() {
                getLogger().debug("Judgment 2 Button pressed!");
                this->callback(3);
            })->get_transform()->GetParent()->GetComponent<LayoutElement*>()->set_preferredWidth(50.0f);


            BeatSaberUI::CreateUIButton(settingsLayoutTransform, "Judgment 3",
                                        [&]() {
                getLogger().debug("Judgment 3 Button pressed!");
                this->callback(2);
            })->get_transform()->GetParent()->GetComponent<LayoutElement*>()->set_preferredWidth(50.0f);

            BeatSaberUI::CreateUIButton(settingsLayoutTransform, "Judgment 4",
                                        [&]() {
                getLogger().debug("Judgment 4 Button pressed!");
                this->callback(4);
            })->get_transform()->GetParent()->GetComponent<LayoutElement*>()->set_preferredWidth(50.0f);

            BeatSaberUI::CreateUIButton(settingsLayoutTransform, "Judgment 5",
                                        [&]() {
                getLogger().debug("Judgment 5 Button pressed!");
                this->callback(5);
            })->get_transform()->GetParent()->GetComponent<LayoutElement*>()->set_preferredWidth(50.0f);

            BeatSaberUI::CreateUIButton(settingsLayoutTransform, "Judgment 6",
                                        [&]() {
                getLogger().debug("Judgment 6 Button pressed!");
                this->callback(6);
            })->get_transform()->GetParent()->GetComponent<LayoutElement*>()->set_preferredWidth(50.0f);
        }
    }

    void ConfigViewController::set_selectCallback(std::function<void(int)> callback) {
        this->callback = callback;
    }
}