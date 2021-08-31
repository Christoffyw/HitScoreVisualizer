#pragma once

#include <string>
#include <unordered_map>

<<<<<<< Updated upstream
=======
#include "modloader/shared/modloader.hpp"
// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/byref.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Color.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/register.hpp"
>>>>>>> Stashed changes
#include <string>
#include <iostream>


#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "config-utils/shared/config-utils.hpp"
<<<<<<< Updated upstream
=======
#include "UnityEngine/Color.hpp"

typedef enum ConfigType {
    CONFIG_TYPE_STANDARD
} ConfigType_t;

#define COLOR_ARRAY_LENGTH 4

std::optional<const int> getInt(rapidjson::Value& obj, std::string_view fieldName, bool required = false);
std::optional<float> getFloat(rapidjson::Value& obj, std::string_view fieldName, bool required = false);
std::optional<const char*> getString(rapidjson::Value& obj, std::string_view fieldName, bool required = false);
std::optional<std::string> getText(rapidjson::Value& obj, std::string_view fieldName, bool required = false);
std::optional<bool> getBool(rapidjson::Value& obj, std::string_view fieldName, bool required = false);

class Judgment {
public:
    // Threshold that the score must be greater than or equal to.
    int threshold;
    // HSV specific text to display.
    // Supports TMP text and % operators.
    std::optional<std::string> text;
    // The color to use, if it is valid.
    std::optional<UnityEngine::Color> color;
    // Whether to fade between colors, if possible.
    std::optional<bool> fade;


    void SetText(std::string text, UnityEngine::Color color, int threshold = 0, bool fade = false);
    // private:
    //     // HSV specific text to display.
    //     // Supports TMP text and % operators.
    //     std::optional<std::string> text;
};

class Segment {
public:
    // Threshold that the score portion must be greater than or equal to.
    int threshold;
    // TMP text to display.
    std::optional<std::string> text;


    void SetText(std::string text, int threshold = 0) {
        this->threshold = threshold;
        this->text.emplace(text);
    }
};

class TimeSegment {
public:
    // Threshold that the score portion must be greater than or equal to.
    float threshold;
    // TMP text to display.
    std::optional<std::string> text;


    void SetText(std::string text, float threshold = 0) {
        this->threshold = threshold;
        this->text.emplace(text);
    }
};

class HSVConfig {
public:
    // True: Will cause the config to be overwritten on updates
    bool isDefaultConfig;
    // True: Will use fixedPosX, fixedPosY, fixedPosZ to anchor the visualization in place.
    // Works for images.
    bool useFixedPos;
    // Fixed X Position
    float fixedPosX;
    // Fixed Y Position
    float fixedPosY;
    // Fixed Z Position
    float fixedPosZ;

    int timeDependencyDecimalPrecision;
    int timeDependencyDecimalOffset;

    std::vector<Judgment> judgments;
    std::vector<Segment> beforeCutAngleJudgments;
    std::vector<Segment> accuracyJudgments;
    std::vector<Segment> afterCutAngleJudgments;
    std::vector<TimeSegment> timeDependencyJudgments;

    // Config type
    ConfigType_t type;

    void WriteToConfig(ConfigDocument& config);
    void SetToDefault();
};

class ConfigHelper {
public:
    static void RestoreConfig(std::string_view newPath);
    static void BackupConfig(ConfigDocument& config, std::string_view newPath);
    static bool LoadConfig(HSVConfig& con, ConfigDocument& config);
    static void AddJSONSegment(rapidjson::Document::AllocatorType& allocator, rapidjson::Document::ValueType& arr, Segment& s);
    static void CreateJSONSegments(rapidjson::MemoryPoolAllocator<>& allocator, ConfigDocument& config, std::vector<Segment>& vector, std::string_view name);
    static void AddJSONTimeSegment(rapidjson::Document::AllocatorType& allocator, rapidjson::Document::ValueType& arr, TimeSegment& s);
    static void CreateJSONTimeSegments(rapidjson::MemoryPoolAllocator<>& allocator, ConfigDocument& config, std::vector<TimeSegment>& vector, std::string_view name);
    static void AddJSONJudgment(rapidjson::MemoryPoolAllocator<>& allocator, rapidjson::Document::ValueType& arr, Judgment& j);
};
>>>>>>> Stashed changes

DECLARE_CONFIG(PluginConfig,
        CONFIG_VALUE(IsEnabled, bool, "Mod Enabled", true);

<<<<<<< Updated upstream
CONFIG_VALUE(IsEnabled, bool, "Mod Enabled", true);
CONFIG_VALUE(MaxSpeed, float, "Max Scroll Speed", 600);
CONFIG_VALUE(Accel, float, "Scroll Acceleration", 1.5);
CONFIG_VALUE(IsLinear, bool, "Scrolling is Linear", true);
    CONFIG_INIT_FUNCTION(
            CONFIG_INIT_VALUE(MaxSpeed);
            CONFIG_INIT_VALUE(Accel);
            CONFIG_INIT_VALUE(IsLinear);
            CONFIG_INIT_VALUE(IsEnabled);
    )
=======
        CONFIG_INIT_FUNCTION(
            CONFIG_INIT_VALUE(IsEnabled);
        )
>>>>>>> Stashed changes
)