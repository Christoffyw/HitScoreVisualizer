#pragma once

#include "TokenizedText.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Vector3.hpp"
#include "rapidjson-macros/shared/macros.hpp"

namespace HSV {
    struct FlexibleFloat : TypeOptions<int, float> {
        operator float() const {
            if (auto v = GetValue<float>())
                return *v;
            return *GetValue<int>();
        }
        using TypeOptions<int, float>::TypeOptions;
    };

    // why cordl why
    struct EquatableColor : UnityEngine::Color {
        bool operator==(EquatableColor const& rhs) const { return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a; };
        EquatableColor(UnityEngine::Color const& rhs) : UnityEngine::Color(rhs) {}
        EquatableColor() = default;
    };
    struct EquatableVec3 : UnityEngine::Vector3 {
        bool operator==(EquatableVec3 const& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; };
        EquatableVec3(UnityEngine::Vector3 const& rhs) : UnityEngine::Vector3(rhs) {}
        EquatableVec3() = default;
    };

    DECLARE_JSON_CLASS(Vector3,
        NAMED_VALUE(FlexibleFloat, X, "x");
        NAMED_VALUE(FlexibleFloat, Y, "y");
        NAMED_VALUE(FlexibleFloat, Z, "z");
    )

    DECLARE_JSON_CLASS(Judgement,
        NAMED_VALUE_DEFAULT(int, Threshold, 0, "threshold");
        NAMED_VALUE(std::string, UnprocessedText, "text");
        NAMED_VECTOR(FlexibleFloat, UnprocessedColor, "color");
        NAMED_VALUE_OPTIONAL(bool, Fade, "fade");
        DESERIALIZE_ACTION(0,
            self->Text = TokenizedText(self->UnprocessedText);
            self->Color = UnityEngine::Color(self->UnprocessedColor[0], self->UnprocessedColor[1], self->UnprocessedColor[2], self->UnprocessedColor[3]);
        )
       public:
        TokenizedText Text;
        EquatableColor Color;
    )

    DECLARE_JSON_CLASS(Segment,
        NAMED_VALUE_DEFAULT(int, Threshold, 0, "threshold");
        NAMED_VALUE(std::string, Text, "text");
    )

    DECLARE_JSON_CLASS(FloatSegment,
        NAMED_VALUE_DEFAULT(FlexibleFloat, Threshold, 0, "threshold");
        NAMED_VALUE(std::string, Text, "text");
    )

    DECLARE_JSON_CLASS(Config,
        NAMED_VALUE_OPTIONAL(int, DefaultVersion, "defaultConfigVersion");
        NAMED_VALUE_DEFAULT(bool, IsDefault, false, "isDefaultConfig");
        DESERIALIZE_ACTION(UpgradeDefault,
            if (self->IsDefault && (!self->DefaultVersion || self->DefaultVersion.value() < 1))
                throw JSONException("default config was older default version");
        )
        NAMED_VECTOR(Judgement, Judgements, "judgments");
        DESERIALIZE_ACTION(NeedsJudgements,
            if (self->Judgements.size() < 1)
                throw JSONException("no judgements found in config");
        )
        NAMED_VECTOR_DEFAULT(Judgement, ChainHeadJudgements, {}, "chainHeadJudgments");
        DESERIALIZE_ACTION(NeedsChainHeads,
            self->HasChainHead = self->ChainHeadJudgements.size() > 0;
            if (self->IsDefault && !self->HasChainHead)
                throw JSONException("default config did not contain chain head judgements");
        )
        NAMED_VALUE_OPTIONAL(Judgement, ChainLinkDisplay, "chainLinkDisplay");
        DESERIALIZE_ACTION(NeedsChainLinks,
            self->HasChainLink = self->ChainLinkDisplay.has_value();
            if (self->IsDefault && !self->HasChainLink)
                throw JSONException("default config did not contain chain link judgement");
        )
        NAMED_VECTOR_DEFAULT(Segment, BeforeCutAngleSegments, {}, "beforeCutAngleJudgments");
        NAMED_VECTOR_DEFAULT(Segment, AccuracySegments, {}, "accuracyJudgments");
        NAMED_VECTOR_DEFAULT(Segment, AfterCutAngleSegments, {}, "afterCutAngleJudgments");
        NAMED_VECTOR_DEFAULT(FloatSegment, TimeDependenceSegments, {}, "timeDependencyJudgments");
        NAMED_VALUE_OPTIONAL(FlexibleFloat, FixedPosX, "fixedPosX");
        NAMED_VALUE_OPTIONAL(FlexibleFloat, FixedPosY, "fixedPosY");
        NAMED_VALUE_OPTIONAL(FlexibleFloat, FixedPosZ, "fixedPosZ");
        NAMED_VALUE_OPTIONAL(bool, UseFixedPos, "useFixedPos");
        NAMED_VALUE_OPTIONAL(Vector3, UnprocessedFixedPos, "fixedPosition");
        NAMED_VALUE_OPTIONAL(Vector3, UnprocessedPosOffset, "targetPositionOffset");
        DESERIALIZE_ACTION(ConvertPositions,
            if (self->UseFixedPos.has_value() && self->UseFixedPos.value())
                self->FixedPos = UnityEngine::Vector3(self->FixedPosX.value_or(0), self->FixedPosY.value_or(0), self->FixedPosZ.value_or(0));
            else if (self->UnprocessedFixedPos.has_value())
                self->FixedPos = UnityEngine::Vector3(self->UnprocessedFixedPos->X, self->UnprocessedFixedPos->Y, self->UnprocessedFixedPos->Z);
            if (self->UnprocessedPosOffset)
                self->PosOffset = UnityEngine::Vector3(self->UnprocessedPosOffset->X, self->UnprocessedPosOffset->Y, self->UnprocessedPosOffset->Z);
        )
        NAMED_VALUE_DEFAULT(int, TimeDependenceDecimalPrecision, 1, "timeDependencyDecimalPrecision");
        NAMED_VALUE_DEFAULT(int, TimeDependenceDecimalOffset, 2, "timeDependencyDecimalOffset");
       public:
        std::optional<EquatableVec3> FixedPos;
        std::optional<EquatableVec3> PosOffset;
        bool HasChainHead;
        bool HasChainLink;
    )

    DECLARE_JSON_CLASS(GlobalConfig,
        NAMED_VALUE_DEFAULT(bool, ModEnabled, true, "isEnabled");
        NAMED_VALUE_DEFAULT(std::string, SelectedConfig, "", "selectedConfig");
        NAMED_VALUE_DEFAULT(bool, HideUntilDone, false, "hideUntilCalculated");
       public:
        std::optional<Config> CurrentConfig = std::nullopt;

        bool GetActive() { return ModEnabled && CurrentConfig.has_value(); }
    )
}
