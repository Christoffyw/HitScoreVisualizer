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

ChainHeadJudgement& GetBestChainHeadJudgement(std::vector<ChainHeadJudgement>& chainHeadJudgements, int comparison) {
    ChainHeadJudgement* best = nullptr;
    for(auto& chainHeadJudgement : chainHeadJudgements) {
        if(comparison >= chainHeadJudgement.Threshold && (!best || chainHeadJudgement.Threshold > best->Threshold))
            best = &chainHeadJudgement;
    }
    return best ? *best : chainHeadJudgements.back();
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

std::string GetChainHeadJudgementText(ChainHeadJudgement& judgement, int score, int before, int after, int accuracy, float timeDependence) {
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

std::string GetChainLinkText(ChainLink& link, int score, int before, int after, int accuracy, float timeDependence) {
    auto& text = link.Text;

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

UnityEngine::Color GetJudgementColor(Judgement& judgement, int score) {
    if(!judgement.Fade || !judgement.Fade.value())
        return judgement.Color;
    // get the lowest judgement with a higher threshold
    Judgement* best = nullptr;
    for(auto& judgement : globalConfig.CurrentConfig->Judgements) {
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

UnityEngine::Color GetChainHeadJudgementColor(ChainHeadJudgement& judgement, int score) {
    if(!judgement.Fade || !judgement.Fade.value())
        return judgement.Color;
    // get the lowest judgement with a higher threshold
    ChainHeadJudgement* best = nullptr;
    for(auto& judgement : globalConfig.CurrentConfig->ChainHeadJudgements) {
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

void ChainScoreWithoutMultiplier(CutScoreBuffer* cutScoreBuffer, float cutDistanceToCenter, ByRef<int> beforeCutRawScore, ByRef<int> afterCutRawScore, ByRef<int> cutDistanceRawScore)
{
    beforeCutRawScore = ((cutScoreBuffer != nullptr) ? round(70 * cutScoreBuffer->get_beforeCutSwingRating()) : 0);
    afterCutRawScore = 0;
    float num = 1 - UnityEngine::Mathf::Clamp01(cutDistanceToCenter / 0.3f);
    cutDistanceRawScore = round(15 * num);
}

void RawScoreWithoutMultiplier(CutScoreBuffer* cutScoreBuffer, float cutDistanceToCenter, ByRef<int> beforeCutRawScore, ByRef<int> afterCutRawScore, ByRef<int> cutDistanceRawScore)
{
    beforeCutRawScore = ((cutScoreBuffer != nullptr) ? round(70 * cutScoreBuffer->get_beforeCutSwingRating()) : 0);
    afterCutRawScore = ((cutScoreBuffer != nullptr) ? round(30 * cutScoreBuffer->get_afterCutSwingRating()) : 0);
    float num = 1 - UnityEngine::Mathf::Clamp01(cutDistanceToCenter / 0.3f);
    cutDistanceRawScore = round(15 * num);
}

void UpdateScoreEffect(FlyingScoreEffect* flyingScoreEffect, int total, int before, int after, int accuracy, float timeDependence, NoteData::ScoringType scoringType) {

    if(scoringType == NoteData::ScoringType::BurstSliderHead) {
        auto& judgement = GetBestChainHeadJudgement(globalConfig.CurrentConfig->ChainHeadJudgements, total);

        auto text = GetChainHeadJudgementText(judgement, total, before, after, accuracy, timeDependence);
        auto color = GetChainHeadJudgementColor(judgement, total);

        flyingScoreEffect->text->set_text(text);
        flyingScoreEffect->text->set_color(color);
        flyingScoreEffect->color = color;
    }
    else if(scoringType == NoteData::ScoringType::BurstSliderElement) {
        auto& link = globalConfig.CurrentConfig->ChainLinkDisplay;

        auto text = GetChainLinkText(link, total, before, after, accuracy, timeDependence);
        auto color = link.Color;

        flyingScoreEffect->text->set_text(text);
        flyingScoreEffect->text->set_color(color);
        flyingScoreEffect->color = color;
    }
    else {
        auto& judgement = GetBestJudgement(globalConfig.CurrentConfig->Judgements, total);

        auto text = GetJudgementText(judgement, total, before, after, accuracy, timeDependence);
        auto color = GetJudgementColor(judgement, total);

        flyingScoreEffect->text->set_text(text);
        flyingScoreEffect->text->set_color(color);
        flyingScoreEffect->color = color;
    }
}

void Judge(CutScoreBuffer* cutScoreBuffer, FlyingScoreEffect* flyingScoreEffect, NoteCutInfo noteCutInfo) {
    if(cutScoreBuffer == nullptr) {
        getLogger().info("CutScoreBuffer is null");
        return;
    }
    if(flyingScoreEffect == nullptr) {
        getLogger().info("FlyingScoreEffect is null");
        return;
    }

    int before, after, accuracy, total;

    //<---Check Note Definition--->

    NoteData::ScoringType scoringType = cutScoreBuffer->get_noteCutInfo().noteData->get_scoringType();

    //Chain Tail
    if(scoringType == NoteData::ScoringType::BurstSliderElement) {
        total = cutScoreBuffer->noteScoreDefinition->fixedCutScore;
    }
    //Chain Head
    else if(scoringType == NoteData::ScoringType::BurstSliderHead) {
        ChainScoreWithoutMultiplier(cutScoreBuffer, noteCutInfo.cutDistanceToCenter, byref(before), byref(after), byref(accuracy));
        total = before + after + accuracy;
    }
    else {
        RawScoreWithoutMultiplier(cutScoreBuffer, noteCutInfo.cutDistanceToCenter, byref(before), byref(after), byref(accuracy));
        total = before + after + accuracy;
    }

    //1.20 removed the ScoreModel method so I had to recreate it

    float timeDependence = std::abs(noteCutInfo.cutNormal.z);

    UpdateScoreEffect(flyingScoreEffect, total, before, after, accuracy, timeDependence, scoringType);
}