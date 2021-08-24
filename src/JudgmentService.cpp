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
    judgment0.threshold = 110;
    judgment0.text = "Fantastic%n%s";
    judgment0.color = Color(0,1,0,1);
    judgment0.fade = false;

    Judgment judgment1;
    judgment1.threshold = 101;
    judgment1.text = "<size=80%>Excellent</size>%n%s";
    judgment1.color = Color(0.69,1,0,1);
    judgment1.fade = false;

    Judgment judgment2;
    judgment2.threshold = 90;
    judgment2.text = "<size=80%>Great</size>%n%s"; //115 <= 110 -> YES
    judgment2.color = Color(1, 0.98, 0, 1);
    judgment2.fade = false;

    Judgment judgment3;
    judgment3.threshold = 80;
    judgment3.text = "<size=80%>Good</size>%n%s";
    judgment3.color = Color(1, 0.6, 0, 1);
    judgment3.fade = true;

    Judgment judgment4;
    judgment4.threshold = 60;
    judgment4.text = "<size=80%>Decent</size>%n%s";
    judgment4.color = Color::get_red();
    judgment4.fade = true;

    Judgment judgment5;
    judgment5.threshold = 0;
    judgment5.text = "<size=80%>Way Off</size>%n%s";
    judgment5.color = Color(0.5, 0, 0, 1);
    judgment5.fade = true;

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
        if(judgment.threshold <= score) {
            chosen = judgment;
        }
    }

    return DisplayModeFormat(score, before, after, accuracy, timeDependence, chosen);
}

Color HitScore::JudgmentService::JudgeColor(int score, int before, int after, int accuracy, float timeDependence) {

    Judgment chosen;

    for(auto& judgment : judgments) {
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
