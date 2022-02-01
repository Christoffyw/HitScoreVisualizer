#include "Main.hpp"

#include "GlobalNamespace/ScoreModel.hpp"

#include "TMPro/TextMeshPro.hpp"

#include <sstream>
#include <iomanip>

using namespace GlobalNamespace;
using namespace HSV;

Judgement& GetBestJudgement(std::vector<Judgement>& judgements, int comparison) {
    for(auto& judgement : judgements) {
        if(comparison >= judgement.Threshold)
            return judgement;
    }
    return judgements[judgements.size() - 1];
}

std::string GetBestSegmentText(std::vector<Segment>& segments, int comparison) {
    for(auto& segment : segments) {
        if(comparison >= segment.Threshold)
            return segment.Text;
    }
    return "";
}

std::string GetBestFloatSegmentText(std::vector<FloatSegment>& segments, int comparison) {
    for(auto& segment : segments) {
        if(comparison >= segment.Threshold)
            return segment.Text;
    }
    return "";
}

std::string TimeDependenceString(float timeDependence) {
    int multiplier = std::pow(10, globalConfig.CurrentConfig->TimeDependenceDecimalOffset);
    std::stringstream ss;
    ss << std::fixed << std::setprecision(globalConfig.CurrentConfig->TimeDependenceDecimalPrecision) << (timeDependence * multiplier);
    return ss.str();
}

std::string GetJudgementText(Judgement& judgement, int score, int before, int after, int accuracy, float timeDependence) {
    auto& text = judgement.Text;

    text.set_beforeCut(std::to_string(before));
    text.set_accuracy(std::to_string(accuracy));
    text.set_afterCut(std::to_string(after));
    text.set_score(std::to_string(score));
    text.set_percent(std::to_string(std::round(100 * (float) score / 115)));
    text.set_timeDependency(TimeDependenceString(timeDependence));
    text.set_beforeCutSegment(GetBestSegmentText(globalConfig.CurrentConfig->BeforeCutAngleSegments, before));
    text.set_accuracySegment(GetBestSegmentText(globalConfig.CurrentConfig->AccuracySegments, accuracy));
    text.set_afterCutSegment(GetBestSegmentText(globalConfig.CurrentConfig->AfterCutAngleSegments, after));
    text.set_timeDependencySegment(GetBestFloatSegmentText(globalConfig.CurrentConfig->TimeDependenceSegments, timeDependence));

    return text.Join();
}

void UpdateScoreEffect(FlyingScoreEffect* flyingScoreEffect, int total, int before, int after, int accuracy, float timeDependence) {
    auto& judgement = GetBestJudgement(globalConfig.CurrentConfig->Judgements, total);

    flyingScoreEffect->text->set_text(CSTR(GetJudgementText(judgement, total, before, after, accuracy, timeDependence)));
    flyingScoreEffect->text->set_color(judgement.Color);
    flyingScoreEffect->text->set_richText(true);
    flyingScoreEffect->text->set_enableWordWrapping(false);
    flyingScoreEffect->text->set_overflowMode(TMPro::TextOverflowModes::Overflow);
    flyingScoreEffect->color = judgement.Color;
}

void Judge(ISaberSwingRatingCounter* swingCounter, FlyingScoreEffect* flyingScoreEffect, NoteCutInfo noteCutInfo) {

    int before, after, accuracy;
    ScoreModel::RawScoreWithoutMultiplier(swingCounter, noteCutInfo.cutDistanceToCenter, byref(before), byref(after), byref(accuracy));
    int total = before + after + accuracy;
    float timeDependence = std::abs(noteCutInfo.cutNormal.z);

    UpdateScoreEffect(flyingScoreEffect, total, before, after, accuracy, timeDependence);
}