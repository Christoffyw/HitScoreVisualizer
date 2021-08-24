#include "JudgmentService.hpp"
#include "PluginConfig.hpp"
#include "main.hpp"

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
#include "UnityEngine/AnimationCurve.hpp"

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
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounter.hpp"
#include "GlobalNamespace/ScoreModel.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/byref.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/register.hpp"
#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "Zenject/DiContainer.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace HMUI;
using namespace GlobalNamespace;
using namespace QuestUI;
using namespace UnityEngine;

DEFINE_TYPE(HitScore, JudgmentService);

void HitScore::JudgmentService::ctor() {
    Judgment judgment0;
    judgment0.threshold = getPluginConfig().J1Threshold.GetValue();
    judgment0.text = getPluginConfig().J1Text.GetValue();
    judgment0.color = getPluginConfig().J1Color.GetValue();
    judgment0.enabled = getPluginConfig().J1Enabled.GetValue();

    Judgment judgment1;
    judgment1.threshold = getPluginConfig().J2Threshold.GetValue();
    judgment1.text = getPluginConfig().J2Text.GetValue();
    judgment1.color = getPluginConfig().J2Color.GetValue();
    judgment1.enabled = getPluginConfig().J2Enabled.GetValue();

    Judgment judgment2;
    judgment2.threshold = getPluginConfig().J3Threshold.GetValue();
    judgment2.text = getPluginConfig().J3Text.GetValue();
    judgment2.color = getPluginConfig().J3Color.GetValue();
    judgment2.enabled = getPluginConfig().J3Enabled.GetValue();

    Judgment judgment3;
    judgment3.threshold = getPluginConfig().J4Threshold.GetValue();
    judgment3.text = getPluginConfig().J4Text.GetValue();
    judgment3.color = getPluginConfig().J4Color.GetValue();
    judgment3.enabled = getPluginConfig().J4Enabled.GetValue();

    Judgment judgment4;
    judgment4.threshold = getPluginConfig().J5Threshold.GetValue();
    judgment4.text = getPluginConfig().J5Text.GetValue();
    judgment4.color = getPluginConfig().J5Color.GetValue();
    judgment4.enabled = getPluginConfig().J5Enabled.GetValue();

    Judgment judgment5;
    judgment5.threshold = getPluginConfig().J6Threshold.GetValue();
    judgment5.text = getPluginConfig().J6Text.GetValue();
    judgment5.color = getPluginConfig().J6Color.GetValue();
    judgment5.enabled = getPluginConfig().J6Enabled.GetValue();

    judgments.push_back(judgment5);
    judgments.push_back(judgment4);
    judgments.push_back(judgment3);
    judgments.push_back(judgment2);
    judgments.push_back(judgment1);
    judgments.push_back(judgment0);
}



std::string HitScore::JudgmentService::JudgeText(int score, int before, int after, int accuracy, float timeDependence) {

    Judgment chosen;

    for(auto& judgment : judgments) {
        if(!judgment.enabled)
            continue;
        if(judgment.threshold <= score) {
            chosen = judgment;
        }
    }

    return DisplayModeFormat(score, before, after, accuracy, timeDependence, chosen);
}

Color HitScore::JudgmentService::JudgeColor(int score, int before, int after, int accuracy, float timeDependence) {

    Judgment chosen;

    for(auto& judgment : judgments) {
        if(!judgment.enabled)
            continue;
        if(judgment.threshold <= score) {
            chosen = judgment;
        }
    }

    return chosen.color;
}

std::string HitScore::JudgmentService::DisplayModeFormat(int score, int before, int after, int accuracy, float timeDependence, const Judgment& judgment) {
    auto formatString = judgment.text;
    std::string buildString;
    auto nextPercentIndex = formatString.find('%');
    if(nextPercentIndex == -1)
        return judgment.text;
    while(nextPercentIndex != -1) {
        buildString.append(formatString.substr(0, nextPercentIndex));
        if(formatString.length() == nextPercentIndex + 1) {
            formatString += " ";
        }

        auto specifier = formatString[nextPercentIndex + 1];

        switch (tolower(specifier)) {
            case 'b':
                buildString.append(std::to_string(before));
                break;
            case 'c':
                buildString.append(std::to_string(accuracy));
                break;
            case 'a':
                buildString.append(std::to_string(after));
                break;
            case 's':
                buildString.append(std::to_string(score));
                break;
            case '%':
                buildString.append("%");
                break;
            case '>':
                buildString.append("%>");
                break;
            case 'n':
                buildString.append("\n");
                break;
            default:
                buildString.append(("%" + to_string(specifier)));
                break;
        }

        formatString = formatString.erase(0, nextPercentIndex + 2);
        nextPercentIndex = formatString.find('%');
    }

    getLogger().info("%s", buildString.c_str());
    return buildString;
}
