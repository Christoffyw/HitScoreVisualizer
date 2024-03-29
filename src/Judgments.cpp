#include "Main.hpp"

#include "GlobalNamespace/ScoreModel.hpp"
#include "GlobalNamespace/ScoreModel_NoteScoreDefinition.hpp"
#include "GlobalNamespace/IReadonlyCutScoreBuffer.hpp"
#include "GlobalNamespace/CutScoreBuffer.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "UnityEngine/Mathf.hpp"

#include "TMPro/TextMeshPro.hpp"

#include <sstream>
#include <iomanip>
#include <math.h>

using namespace GlobalNamespace;
using namespace HSV;

Judgement& GetBestJudgement(std::vector<Judgement>& judgements, int comparison) {
    Judgement* best = nullptr;
    for(auto& judgement : judgements) {
        if(comparison >= judgement.Threshold && (!best || judgement.Threshold > best->Threshold))
            best = &judgement;
    }
    return best ? *best : judgements.back();
}

std::string GetBestSegmentText(std::vector<Segment>& segments, int comparison) {
    Segment* best = nullptr;
    for(auto& segment : segments) {
        if(comparison >= segment.Threshold && (!best || segment.Threshold > best->Threshold))
            best = &segment;
    }
    return best ? best->Text : "";
}

std::string GetBestFloatSegmentText(std::vector<FloatSegment>& segments, float comparison) {
    FloatSegment* best = nullptr;
    for(auto& segment : segments) {
        if(comparison >= segment.Threshold && (!best || segment.Threshold > best->Threshold))
            best = &segment;
    }
    return best ? best->Text : "";
}

std::string TimeDependenceString(float timeDependence) {
    int multiplier = std::pow(10, globalConfig.CurrentConfig->TimeDependenceDecimalOffset);
    std::stringstream ss;
    ss << std::fixed << std::setprecision(globalConfig.CurrentConfig->TimeDependenceDecimalPrecision) << (timeDependence * multiplier);
    return ss.str();
}

std::string GetJudgementText(Judgement& judgement, int score, int before, int after, int accuracy, float timeDependence, int maxScore) {
    auto& text = judgement.Text;

    text.set_beforeCut(std::to_string(before));
    text.set_accuracy(std::to_string(accuracy));
    text.set_afterCut(std::to_string(after));
    text.set_score(std::to_string(score));
    text.set_percent(std::to_string(round(100 * (float) score / maxScore)));
    text.set_timeDependency(TimeDependenceString(timeDependence));
    text.set_beforeCutSegment(GetBestSegmentText(globalConfig.CurrentConfig->BeforeCutAngleSegments, before));
    text.set_accuracySegment(GetBestSegmentText(globalConfig.CurrentConfig->AccuracySegments, accuracy));
    text.set_afterCutSegment(GetBestSegmentText(globalConfig.CurrentConfig->AfterCutAngleSegments, after));
    text.set_timeDependencySegment(GetBestFloatSegmentText(globalConfig.CurrentConfig->TimeDependenceSegments, timeDependence));

    return text.Join();
}

UnityEngine::Color GetJudgementColor(Judgement& judgement, std::vector<Judgement>& judgements, int score) {
    if(!judgement.Fade || !judgement.Fade.value())
        return judgement.Color;
    // get the lowest judgement with a higher threshold
    Judgement* best = nullptr;
    for(auto& judgement : judgements) {
        if(score < judgement.Threshold && (!best || judgement.Threshold < best->Threshold))
            best = &judgement;
    }
    if(!best)
        return judgement.Color;
    int lowerThreshold = judgement.Threshold;
    int higherThreshold = best->Threshold;
    float lerpDistance = ((float) score - lowerThreshold) / (higherThreshold - lowerThreshold);
    auto lowerColor = judgement.Color;
    auto higherColor = best->Color;
    return UnityEngine::Color(
        lowerColor.r + (higherColor.r - lowerColor.r) * lerpDistance,
        lowerColor.g + (higherColor.g - lowerColor.g) * lerpDistance,
        lowerColor.b + (higherColor.b - lowerColor.b) * lerpDistance,
        lowerColor.a + (higherColor.a - lowerColor.a) * lerpDistance
    );
}

void UpdateScoreEffect(FlyingScoreEffect* flyingScoreEffect, int total, int before, int after, int accuracy, float timeDependence, NoteData::ScoringType scoringType) {
    std::string text;
    UnityEngine::Color color;

    int maxScore = ScoreModel::GetNoteScoreDefinition(scoringType)->get_maxCutScore();

    if(scoringType == NoteData::ScoringType::BurstSliderElement) {
        auto&& judgement = globalConfig.CurrentConfig->ChainLinkDisplay.value_or(GetBestJudgement(globalConfig.CurrentConfig->Judgements, total));

        text = GetJudgementText(judgement, total, before, after, accuracy, timeDependence, maxScore);
        color = judgement.Color;
    } else if(scoringType == NoteData::ScoringType::BurstSliderHead) {
        auto& judgementVector = globalConfig.CurrentConfig->ChainHeadJudgements;
        auto& judgement = GetBestJudgement(judgementVector, total);

        text = GetJudgementText(judgement, total, before, after, accuracy, timeDependence, maxScore);
        color = GetJudgementColor(judgement, judgementVector, total);
    } else {
        auto& judgementVector = globalConfig.CurrentConfig->Judgements;
        auto& judgement = GetBestJudgement(judgementVector, total);

        text = GetJudgementText(judgement, total, before, after, accuracy, timeDependence, maxScore);
        color = GetJudgementColor(judgement, judgementVector, total);
    }

    flyingScoreEffect->text->set_text(text);
    flyingScoreEffect->text->set_color(color);
    flyingScoreEffect->color = color;
}

void Judge(CutScoreBuffer* cutScoreBuffer, FlyingScoreEffect* flyingScoreEffect, NoteCutInfo& noteCutInfo) {
    if(!cutScoreBuffer) {
        LOG_INFO("CutScoreBuffer is null");
        return;
    }
    if(!flyingScoreEffect) {
        LOG_INFO("FlyingScoreEffect is null");
        return;
    }

    if(!cutScoreBuffer->get_isFinished() && globalConfig.HideUntilDone) {
        flyingScoreEffect->text->set_text("");
        return;
    }

    // get scores for each part of the cut
    int before = cutScoreBuffer->get_beforeCutScore();
    int after = cutScoreBuffer->get_afterCutScore();
    int accuracy = cutScoreBuffer->get_centerDistanceCutScore();
    int total = cutScoreBuffer->get_cutScore();
    float timeDependence = std::abs(noteCutInfo.cutNormal.z);

    NoteData::ScoringType scoringType = cutScoreBuffer->get_noteCutInfo().noteData->get_scoringType();

    UpdateScoreEffect(flyingScoreEffect, total, before, after, accuracy, timeDependence, scoringType);
}
