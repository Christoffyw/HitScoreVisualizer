#pragma once

#include "TokenizedText.hpp"
#include "json/TypeMacros.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Vector3.hpp"

DECLARE_JSON_CLASS(HSV, Vector3,
    float X;
    float Y;
    float Z;
)

DECLARE_JSON_CLASS(HSV, Judgement,
    int Threshold;
    std::string UnprocessedText;
    std::vector<float> UnprocessedColor;
    std::optional<bool> Fade;
    TokenizedText Text;
    UnityEngine::Color Color;
)

DECLARE_JSON_CLASS(HSV, Segment,
    int Threshold;
    std::string Text;
)

DECLARE_JSON_CLASS(HSV, FloatSegment,
    float Threshold;
    std::string Text;
)

DECLARE_JSON_CLASS(HSV, Config,
    bool IsDefault;
    std::vector<Judgement> Judgements;
    std::vector<Judgement> ChainHeadJudgements;
    std::optional<Judgement> ChainLinkDisplay;
    std::vector<Segment> BeforeCutAngleSegments;
    std::vector<Segment> AccuracySegments;
    std::vector<Segment> AfterCutAngleSegments;
    std::vector<FloatSegment> TimeDependenceSegments;
    std::optional<float> FixedPosX;
    std::optional<float> FixedPosY;
    std::optional<float> FixedPosZ;
    std::optional<bool> UseFixedPos;
    std::optional<Vector3> UnprocessedFixedPos;
    std::optional<Vector3> UnprocessedPosOffset;
    std::optional<UnityEngine::Vector3> FixedPos;
    std::optional<UnityEngine::Vector3> PosOffset;
    int TimeDependenceDecimalPrecision;
    int TimeDependenceDecimalOffset;
)

DECLARE_JSON_CLASS(HSV, GlobalConfig,
    bool ModEnabled = true;
    std::string SelectedConfig = "";
    std::optional<Config> CurrentConfig = std::nullopt;

    bool GetActive() { return ModEnabled && CurrentConfig.has_value(); }
)