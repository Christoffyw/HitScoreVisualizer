#include "JudgmentService.hpp"
#include "PluginConfig.hpp"
#include "main.hpp"
#include "HsvFlyingScoreEffect.hpp"

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
DEFINE_TYPE(HitScore, Judgment);
DEFINE_TYPE(HitScore, HsvFlyingScoreEffect);

Il2CppString* HitScore::JudgmentService::JudgeText(int score, int before, int after, int accuracy, float timeDependence) {
    //Temp Judgment setup
    std::vector< HitScore::Judgment* > judgments;
    Judgment* judgment1 = new Judgment();
    judgment1->threshold = 115;
    judgment1->text = il2cpp_utils::newcsstr("POGGERS");
    judgment1->color = Color::get_red();

    Judgment* judgment2 = new Judgment();
    judgment2->threshold = 90;
    judgment2->text = il2cpp_utils::newcsstr("EH");
    judgment2->color = Color::get_green();

    Judgment* judgment3 = new Judgment();
    judgment3->threshold = 50;
    judgment3->text = il2cpp_utils::newcsstr("BRUH");
    judgment3->color = Color::get_white();

    judgments.push_back(judgment3);
    judgments.push_back(judgment2);
    judgments.push_back(judgment1);


    for(auto judgment : judgments) {
        if(judgment->threshold <= score)  {
            return judgment->text;
        }
    }
    return il2cpp_utils::newcsstr(std::to_string(score));
}

Color HitScore::JudgmentService::JudgeColor(int score, int before, int after, int accuracy, float timeDependence) {
    //Temp Judgment setup
    std::vector< HitScore::Judgment* > judgments;
    Judgment* judgment1 = new Judgment();
    judgment1->threshold = 115;
    judgment1->text = il2cpp_utils::newcsstr("POGGERS");
    judgment1->color = Color::get_red();

    Judgment* judgment2 = new Judgment();
    judgment2->threshold = 90;
    judgment2->text = il2cpp_utils::newcsstr("EH");
    judgment2->color = Color::get_green();

    Judgment* judgment3 = new Judgment();
    judgment3->threshold = 50;
    judgment3->text = il2cpp_utils::newcsstr("BRUH");
    judgment3->color = Color::get_white();

    judgments.push_back(judgment3);
    judgments.push_back(judgment2);
    judgments.push_back(judgment1);


    for(auto judgment : judgments) {
        if(judgment->threshold <= score)  {
            return judgment->color;
        }
    }

    return Color::get_white();
}
