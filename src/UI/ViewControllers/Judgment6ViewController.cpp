#include "main.hpp"
#include "ViewControllers/Judgment6ViewController.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"
#include "questui/shared/QuestUI.hpp"

#include "UnityEngine/Object.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "PluginConfig.hpp"

using namespace HitScore;
using namespace QuestUI;

DEFINE_TYPE(HitScore::ViewControllers, Judgment6ViewController);

namespace HitScore::ViewControllers {
    void Judgment6ViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if (firstActivation && addedToHierarchy)
        {
            UnityEngine::UI::VerticalLayoutGroup* container = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_rectTransform());
            container->set_spacing(0.4f);
            container->GetComponent<UnityEngine::UI::LayoutElement*>()->set_minWidth(125.0);


            //Bool settings
            this->HSconfigcontainer = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(container->get_rectTransform());
            HSconfigcontainer->set_childAlignment(UnityEngine::TextAnchor::UpperCenter);
            HSconfigcontainer->set_childForceExpandHeight(false);
            HSconfigcontainer->set_childControlHeight(true);

            // Enable or Disable Judgment
            AddConfigValueToggle(HSconfigcontainer->get_rectTransform(), getPluginConfig().J6Enabled);

            // Threshold
            AddConfigValueIncrementInt(HSconfigcontainer->get_rectTransform(), getPluginConfig().J6Threshold, 1, 0, 115);

            // Text
            AddConfigValueStringSetting(HSconfigcontainer->get_rectTransform(), getPluginConfig().J6Text);

            // Color
            AddConfigValueColorPicker(HSconfigcontainer->get_rectTransform(), getPluginConfig().J6Color);
        }
    }

    void Judgment6ViewController::DidDeactivate(bool removedFromHierarchy, bool systemScreenDisabling) {
        //Deactivate
    }
}