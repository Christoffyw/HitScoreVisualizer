#pragma once

#include "TokenizedText.hpp"
#include "rapidjson-macros/shared/macros.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Vector3.hpp"

DECLARE_JSON_CLASS(HSV, Vector3,
    NAMED_AUTO_VALUE(float, X, "x");
    NAMED_AUTO_VALUE(float, Y, "y");
    NAMED_AUTO_VALUE(float, Z, "z");
)

DECLARE_JSON_CLASS(HSV, Judgement,
    NAMED_AUTO_VALUE_DEFAULT(int, Threshold, 0, "threshold");
    NAMED_AUTO_VALUE(std::string, UnprocessedText, "text");
    NAMED_AUTO_VECTOR(float, UnprocessedColor, "color");
    NAMED_AUTO_VALUE_OPTIONAL(bool, Fade, "fade");
    DESERIALIZE_ACTION(0,
        outerClass->Text = TokenizedText(outerClass->UnprocessedText);
        outerClass->Color = UnityEngine::Color(outerClass->UnprocessedColor[0], outerClass->UnprocessedColor[1], outerClass->UnprocessedColor[2], outerClass->UnprocessedColor[3]);
    )
    public:
    TokenizedText Text;
    UnityEngine::Color Color;
)

DECLARE_JSON_CLASS(HSV, Segment,
    NAMED_AUTO_VALUE_DEFAULT(int, Threshold, 0, "threshold");
    NAMED_AUTO_VALUE(std::string, Text, "text");
)

DECLARE_JSON_CLASS(HSV, FloatSegment,
    NAMED_AUTO_VALUE_DEFAULT(float, Threshold, 0, "threshold");
    NAMED_AUTO_VALUE(std::string, Text, "text");
)

DECLARE_JSON_CLASS(HSV, Config,
    NAMED_AUTO_VALUE_DEFAULT(bool, IsDefault, false, "isDefaultConfig");
    NAMED_AUTO_VECTOR(HSV::Judgement, Judgements, "judgments");
    DESERIALIZE_ACTION(0,
        if(outerClass->Judgements.size() < 1)
            throw JSONException("no judgements found in config");
    )
    NAMED_AUTO_VECTOR_DEFAULT(HSV::Judgement, ChainHeadJudgements, {}, "chainHeadJudgments");
    DESERIALIZE_ACTION(1,
        outerClass->HasChainHead = outerClass->ChainHeadJudgements.size() > 0;
        if(outerClass->IsDefault && !outerClass->HasChainHead)
            throw JSONException("default config did not contain chain head judgements");
    )
    NAMED_AUTO_VALUE_OPTIONAL(HSV::Judgement, ChainLinkDisplay, "chainLinkDisplay");
    NAMED_AUTO_VECTOR_DEFAULT(HSV::Segment, BeforeCutAngleSegments, {}, "beforeCutAngleJudgments");
    NAMED_AUTO_VECTOR_DEFAULT(HSV::Segment, AccuracySegments, {}, "accuracyJudgments");
    NAMED_AUTO_VECTOR_DEFAULT(HSV::Segment, AfterCutAngleSegments, {}, "afterCutAngleJudgments");
    NAMED_AUTO_VECTOR_DEFAULT(HSV::FloatSegment, TimeDependenceSegments, {}, "timeDependencyJudgments");
    NAMED_AUTO_VALUE_OPTIONAL(float, FixedPosX, "fixedPosX");
    NAMED_AUTO_VALUE_OPTIONAL(float, FixedPosY, "fixedPosY");
    NAMED_AUTO_VALUE_OPTIONAL(float, FixedPosZ, "fixedPosZ");
    NAMED_AUTO_VALUE_OPTIONAL(bool, UseFixedPos, "useFixedPos");
    NAMED_AUTO_VALUE_OPTIONAL(HSV::Vector3, UnprocessedFixedPos, "fixedPosition");
    NAMED_AUTO_VALUE_OPTIONAL(HSV::Vector3, UnprocessedPosOffset, "targetPositionOffset");
    DESERIALIZE_ACTION(2,
        if(outerClass->UseFixedPos.has_value() && outerClass->UseFixedPos.value())
            outerClass->FixedPos = UnityEngine::Vector3(outerClass->FixedPosX.value_or(0), outerClass->FixedPosY.value_or(0), outerClass->FixedPosZ.value_or(0));
        else if(outerClass->UnprocessedFixedPos.has_value())
            outerClass->FixedPos = UnityEngine::Vector3(outerClass->UnprocessedFixedPos->X, outerClass->UnprocessedFixedPos->Y, outerClass->UnprocessedFixedPos->Z);
    )
    DESERIALIZE_ACTION(3,
        if(outerClass->UnprocessedPosOffset)
            outerClass->PosOffset = UnityEngine::Vector3(outerClass->UnprocessedPosOffset->X, outerClass->UnprocessedPosOffset->Y, outerClass->UnprocessedPosOffset->Z);
    )
    NAMED_AUTO_VALUE_DEFAULT(int, TimeDependenceDecimalPrecision, 1, "timeDependencyDecimalPrecision");
    NAMED_AUTO_VALUE_DEFAULT(int, TimeDependenceDecimalOffset, 2, "timeDependencyDecimalOffset");
    public:
    std::optional<UnityEngine::Vector3> FixedPos;
    std::optional<UnityEngine::Vector3> PosOffset;
    bool HasChainHead;
    bool HasChainLink;
)

DECLARE_JSON_CLASS(HSV, GlobalConfig,
    NAMED_AUTO_VALUE_DEFAULT(bool, ModEnabled, true, "isEnabled");
    NAMED_AUTO_VALUE_DEFAULT(std::string, SelectedConfig, "", "selectedConfig");
    public:
    std::optional<HSV::Config> CurrentConfig = std::nullopt;

    bool GetActive() { return ModEnabled && CurrentConfig.has_value(); }
)