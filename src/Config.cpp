#include "json/Config.hpp"
#include "Main.hpp"

#include "TokenizedText.hpp"

DESERIALIZE_METHOD(HSV, Vector3,
    DESERIALIZE_VALUE(X, x, Float)
    DESERIALIZE_VALUE(Y, y, Float)
    DESERIALIZE_VALUE(Z, z, Float)
)

SERIALIZE_METHOD(HSV, Vector3)

DESERIALIZE_METHOD(HSV, Judgement,
    DESERIALIZE_VALUE_DEFAULT(Threshold, threshold, Int, 0)
    DESERIALIZE_VALUE(UnprocessedText, text, String)
    DESERIALIZE_VECTOR_BASIC(UnprocessedColor, color, Float)
    if(UnprocessedColor.size() != 4)
        throw JSONException("color was incorrect length!");
    DESERIALIZE_VALUE_OPTIONAL(Fade, fade, Bool)
    Text = TokenizedText(UnprocessedText);
    Color = UnityEngine::Color(UnprocessedColor[0], UnprocessedColor[1], UnprocessedColor[2], UnprocessedColor[3]);
)

SERIALIZE_METHOD(HSV, Judgement)

DESERIALIZE_METHOD(HSV, Segment,
    DESERIALIZE_VALUE_DEFAULT(Threshold, threshold, Int, 0)
    DESERIALIZE_VALUE(Text, text, String)
)

SERIALIZE_METHOD(HSV, Segment)

DESERIALIZE_METHOD(HSV, FloatSegment,
    DESERIALIZE_VALUE_DEFAULT(Threshold, threshold, Float, 0)
    DESERIALIZE_VALUE(Text, text, String)
)

SERIALIZE_METHOD(HSV, FloatSegment)

DESERIALIZE_METHOD(HSV, Config,
    DESERIALIZE_VALUE_DEFAULT(IsDefault, isDefault, Bool, true)
    DESERIALIZE_VECTOR(Judgements, judgments, Judgement)
    if(Judgements.size() < 1)
        throw JSONException("no judgements found in config!");
    DESERIALIZE_VECTOR_DEFAULT(BeforeCutAngleSegments, beforeCutAngleJudgments, Segment, {})
    DESERIALIZE_VECTOR_DEFAULT(AccuracySegments, accuracyJudgments, Segment, {})
    DESERIALIZE_VECTOR_DEFAULT(AfterCutAngleSegments, afterCutAngleJudgments, Segment, {})
    DESERIALIZE_VECTOR_DEFAULT(TimeDependenceSegments, timeDependencyJudgments, FloatSegment, {})
    DESERIALIZE_VALUE_DEFAULT(FixedPosX, fixedPosX, Float, 0)
    DESERIALIZE_VALUE_DEFAULT(FixedPosY, fixedPosY, Float, 0)
    DESERIALIZE_VALUE_DEFAULT(FixedPosZ, fixedPosZ, Float, 0)
    DESERIALIZE_VALUE_OPTIONAL(UseFixedPos, useFixedPos, Bool)
    DESERIALIZE_CLASS_OPTIONAL(UnprocessedFixedPos, fixedPosition)
    DESERIALIZE_CLASS_OPTIONAL(UnprocessedPosOffset, targetPositionOffset)
    DESERIALIZE_VALUE_DEFAULT(TimeDependenceDecimalPrecision, timeDependencyDecimalPrecision, Int, 1)
    DESERIALIZE_VALUE_DEFAULT(TimeDependenceDecimalOffset, timeDependencyDecimalOffset, Int, 2)
    if(UseFixedPos.has_value() && UseFixedPos.value())
        FixedPos = UnityEngine::Vector3(*FixedPosX, *FixedPosY, *FixedPosZ);
    else if(UnprocessedFixedPos)
        FixedPos = UnityEngine::Vector3(UnprocessedFixedPos->X, UnprocessedFixedPos->Y, UnprocessedFixedPos->Z);
    if(UnprocessedPosOffset)
        PosOffset = UnityEngine::Vector3(UnprocessedPosOffset->X, UnprocessedPosOffset->Y, UnprocessedPosOffset->Z);
)

SERIALIZE_METHOD(HSV, Config)

DESERIALIZE_METHOD(HSV, GlobalConfig,
    DESERIALIZE_VALUE_DEFAULT(ModEnabled, isEnabled, Bool, true)
    DESERIALIZE_VALUE_DEFAULT(SelectedConfig, selectedConfig, String, "HitScoreVisualizerConfig (default).json")
)

SERIALIZE_METHOD(HSV, GlobalConfig,
    SERIALIZE_VALUE(ModEnabled, isEnabled)
    SERIALIZE_VALUE(SelectedConfig, selectedConfig)
)