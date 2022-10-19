#pragma once

#include "TokenizedText.hpp"
#include "rapidjson-macros/shared/macros.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Vector3.hpp"

DECLARE_JSON_CLASS(HSV, Vector3,
    NAMED_VALUE(float, X, "x");
    NAMED_VALUE(float, Y, "y");
    NAMED_VALUE(float, Z, "z");
)

DECLARE_JSON_CLASS(HSV, Judgement,
    NAMED_VALUE_DEFAULT(int, Threshold, 0, "threshold");
    NAMED_VALUE(std::string, UnprocessedText, "text");
    NAMED_VECTOR(float, UnprocessedColor, "color");
    NAMED_VALUE_OPTIONAL(bool, Fade, "fade");
    DESERIALIZE_ACTION(0,
        self->Text = TokenizedText(self->UnprocessedText);
        self->Color = UnityEngine::Color(self->UnprocessedColor[0], self->UnprocessedColor[1], self->UnprocessedColor[2], self->UnprocessedColor[3]);
    )
    public:
    TokenizedText Text;
    UnityEngine::Color Color;
)

DECLARE_JSON_CLASS(HSV, Segment,
    NAMED_VALUE_DEFAULT(int, Threshold, 0, "threshold");
    NAMED_VALUE(std::string, Text, "text");
)

DECLARE_JSON_CLASS(HSV, FloatSegment,
    NAMED_VALUE_DEFAULT(float, Threshold, 0, "threshold");
    NAMED_VALUE(std::string, Text, "text");
)

DECLARE_JSON_CLASS(HSV, Config,
    NAMED_VALUE_DEFAULT(bool, IsDefault, false, "isDefaultConfig");
    NAMED_VECTOR(HSV::Judgement, Judgements, "judgments");
    DESERIALIZE_ACTION(0,
        if(self->Judgements.size() < 1)
            throw JSONException("no judgements found in config");
    )
    NAMED_VECTOR_DEFAULT(HSV::Judgement, ChainHeadJudgements, {}, "chainHeadJudgments");
    DESERIALIZE_ACTION(1,
        self->HasChainHead = self->ChainHeadJudgements.size() > 0;
        if(self->IsDefault && !self->HasChainHead)
            throw JSONException("default config did not contain chain head judgements");
    )
    NAMED_VALUE_OPTIONAL(HSV::Judgement, ChainLinkDisplay, "chainLinkDisplay");
    DESERIALIZE_ACTION(2,
        self->HasChainLink = self->ChainLinkDisplay.has_value();
        if(self->IsDefault && !self->HasChainLink)
            throw JSONException("default config did not contain chain link judgement");
    )
    NAMED_VECTOR_DEFAULT(HSV::Segment, BeforeCutAngleSegments, {}, "beforeCutAngleJudgments");
    NAMED_VECTOR_DEFAULT(HSV::Segment, AccuracySegments, {}, "accuracyJudgments");
    NAMED_VECTOR_DEFAULT(HSV::Segment, AfterCutAngleSegments, {}, "afterCutAngleJudgments");
    NAMED_VECTOR_DEFAULT(HSV::FloatSegment, TimeDependenceSegments, {}, "timeDependencyJudgments");
    NAMED_VALUE_OPTIONAL(float, FixedPosX, "fixedPosX");
    NAMED_VALUE_OPTIONAL(float, FixedPosY, "fixedPosY");
    NAMED_VALUE_OPTIONAL(float, FixedPosZ, "fixedPosZ");
    NAMED_VALUE_OPTIONAL(bool, UseFixedPos, "useFixedPos");
    NAMED_VALUE_OPTIONAL(HSV::Vector3, UnprocessedFixedPos, "fixedPosition");
    NAMED_VALUE_OPTIONAL(HSV::Vector3, UnprocessedPosOffset, "targetPositionOffset");
    DESERIALIZE_ACTION(3,
        if(self->UseFixedPos.has_value() && self->UseFixedPos.value())
            self->FixedPos = UnityEngine::Vector3(self->FixedPosX.value_or(0), self->FixedPosY.value_or(0), self->FixedPosZ.value_or(0));
        else if(self->UnprocessedFixedPos.has_value())
            self->FixedPos = UnityEngine::Vector3(self->UnprocessedFixedPos->X, self->UnprocessedFixedPos->Y, self->UnprocessedFixedPos->Z);
        if(self->UnprocessedPosOffset)
            self->PosOffset = UnityEngine::Vector3(self->UnprocessedPosOffset->X, self->UnprocessedPosOffset->Y, self->UnprocessedPosOffset->Z);
    )
    NAMED_VALUE_DEFAULT(int, TimeDependenceDecimalPrecision, 1, "timeDependencyDecimalPrecision");
    NAMED_VALUE_DEFAULT(int, TimeDependenceDecimalOffset, 2, "timeDependencyDecimalOffset");
    public:
    std::optional<UnityEngine::Vector3> FixedPos;
    std::optional<UnityEngine::Vector3> PosOffset;
    bool HasChainHead;
    bool HasChainLink;
)

DECLARE_JSON_CLASS(HSV, GlobalConfig,
    NAMED_VALUE_DEFAULT(bool, ModEnabled, true, "isEnabled");
    NAMED_VALUE_DEFAULT(std::string, SelectedConfig, "", "selectedConfig");
    public:
    std::optional<HSV::Config> CurrentConfig = std::nullopt;

    bool GetActive() { return ModEnabled && CurrentConfig.has_value(); }
)