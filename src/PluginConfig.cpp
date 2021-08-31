#include "PluginConfig.hpp"
#include "main.hpp"

#define GET(obj, fieldName, method, required) auto itr = obj.FindMember(fieldName.data()); \
if (itr == obj.MemberEnd()) { \
    if (required) { \
        getLogger().warning("Failed to find required field: %s! Could not load config", fieldName.data()); \
    } \
    return std::nullopt; \
} \
return itr->value.method()

std::optional<const int> getInt(rapidjson::Value& obj, std::string_view fieldName, bool required) {
    GET(obj, fieldName, GetInt, required);
}

std::optional<float> getFloat(rapidjson::Value& obj, std::string_view fieldName, bool required) {
    GET(obj, fieldName, GetFloat, required);
}

std::optional<const char*> getString(rapidjson::Value& obj, std::string_view fieldName, bool required) {
    GET(obj, fieldName, GetString, required);
}

std::optional<std::string> getText(rapidjson::Value& obj, std::string_view fieldName, bool required) {
    auto itr = obj.FindMember(fieldName.data());
    if (itr == obj.MemberEnd()) {
        if (required) {
            getLogger().warning("Failed to find required field: %s! Could not load config", fieldName.data());
        }
        return std::nullopt;
    }
    return std::string(itr->value.GetString());
}

std::optional<bool> getBool(rapidjson::Value& obj, std::string_view fieldName, bool required) {
    GET(obj, fieldName, GetBool, required);
}

void Judgment::SetText(std::string text, UnityEngine::Color color, int threshold, bool fade) {
    if (this->threshold != threshold)
        this->threshold = threshold;
    this->text.emplace(text);
    this->color.emplace(color);
    this->fade.emplace(fade);
}

bool getTimeSegments(std::vector<TimeSegment>& out, ConfigDocument& config, std::string_view fieldName) {
    static auto logger = getLogger().WithContext("Config").WithContext("getTimeSegments");
    auto itr = config.FindMember(fieldName.data());
    if (itr == config.MemberEnd()) {
        logger.warning("Failed to find required field: %s! Could not load optional timeSegments", fieldName.data());
        return false;
    }
    auto arr = itr->value.GetArray();
    auto size = arr.Size();
    for (int i = 0; i < size; i++) {
        auto& currentValue = arr[i];
        TimeSegment toAdd;

        if (auto thresh = getFloat(currentValue, "threshold")) {
            toAdd.threshold = *thresh;
        }
        toAdd.text = getString(currentValue, "text");

        // If an image is required but not provided AND no fallback text is available, fail
        // This will bring attention to image only displays failing to have images for all text
        // TODO: Segments do not have text yet!
        // if (requires_image(displayMode) && !toAdd.imagePath) {
        //     if (requires_text(displayMode) && toAdd.text) {
        //         logger.warning("Attempted to load image from: displayMode: %d, but segment: %d had none!", displayMode, i);
        //         logger.info("Will not use an image for this segment");
        //     } else {
        //         logger.warning("Config could not be loaded! displayMode: %d requires an image, but segment: %d had none!", displayMode, i);
        //         return false;
        //     }
        // }
        // Redundant failsafe, should never occur
        if (!toAdd.text) {
            logger.error("Config could not be loaded! Missing text for segment: %d", i);
            return false;
        }
        out.push_back(toAdd);
    }
    return true;
}

bool getSegments(std::vector<Segment>& out, ConfigDocument& config, std::string_view fieldName) {
    static auto logger = getLogger().WithContext("Config").WithContext("getSegments");
    auto itr = config.FindMember(fieldName.data());
    if (itr == config.MemberEnd()) {
        logger.warning("Failed to find required field: %s! Could not load config", fieldName.data());
        return false;
    }
    auto arr = itr->value.GetArray();
    auto size = arr.Size();
    for (int i = 0; i < size; i++) {
        auto& currentValue = arr[i];
        Segment toAdd;

        if (auto thresh = getInt(currentValue, "threshold")) {
            toAdd.threshold = *thresh;
        }
        toAdd.text = getString(currentValue, "text");

        // If an image is required but not provided AND no fallback text is available, fail
        // This will bring attention to image only displays failing to have images for all text
        // TODO: Segments do not have text yet!
        // if (requires_image(displayMode) && !toAdd.imagePath) {
        //     if (requires_text(displayMode) && toAdd.text) {
        //         logger.warning("Attempted to load image from: displayMode: %d, but segment: %d had none!", displayMode, i);
        //         logger.info("Will not use an image for this segment");
        //     } else {
        //         logger.warning("Config could not be loaded! displayMode: %d requires an image, but segment: %d had none!", displayMode, i);
        //         return false;
        //     }
        // }
        // Redundant failsafe, should never occur
        if (!toAdd.text) {
            logger.error("Config could not be loaded! Missing text for segment: %d", i);
            return false;
        }
        out.push_back(toAdd);
    }
    return true;
}

bool getJudgments(std::vector<Judgment>& out, ConfigDocument& obj) {
    static auto logger = getLogger().WithContext("Config").WithContext("getJudgments");
    auto itr = obj.FindMember("judgments");
    if (itr == obj.MemberEnd()) {
        logger.warning("Failed to find required field: judgments! Could not load config");
        return false;
    }
    auto arr = itr->value.GetArray();
    auto size = arr.Size();
    for (int i = 0; i < size; i++) {
        auto& currentValue = arr[i];
        auto& toAdd = out.emplace_back();
        if (auto thresh = getInt(currentValue, "threshold")) {
            toAdd.threshold = *thresh;
        }
        toAdd.text = getText(currentValue, "text");
        toAdd.fade = getBool(currentValue, "fade");

        auto itr = currentValue.FindMember("color");
        if (itr != currentValue.MemberEnd()) {
            auto size = itr->value.GetArray().Size();
            if (size == COLOR_ARRAY_LENGTH) {
                auto arr = itr->value.GetArray();
                UnityEngine::Color c = UnityEngine::Color(arr[0].GetFloat(), arr[1].GetFloat(), arr[2].GetFloat(), arr[3].GetFloat());
                toAdd.color.emplace(c);
            } else {
                toAdd.color = std::nullopt;
            }
        } else {
            toAdd.color = std::nullopt;
        }

        // If text is required but not provided, fail
        if (!toAdd.text) {
            logger.warning("Config could not be loaded! Missing text for judgment: %d", i);
            return false;
        }
        // If color is not provided, revert to default color.
        if (!toAdd.color) {
            logger.warning("Could not load color for judgment: %d, using white!", i);
            toAdd.color.emplace((UnityEngine::Color){0.0f, 0.0f, 0.0f, 0.0f});
        }
    }
    return true;
}

void ConfigHelper::AddJSONJudgment(rapidjson::MemoryPoolAllocator<>& allocator, rapidjson::Document::ValueType& arr, Judgment& j) {
    static auto logger = getLogger().WithContext("Config").WithContext("AddJSONJudgment");
    rapidjson::Value v(rapidjson::kObjectType);
    v.AddMember("threshold", j.threshold, allocator);
    if (j.text) {
        auto text = j.text->c_str();
        logger.debug("Adding text field: %s", text);
        v.AddMember("text", rapidjson::GenericStringRef<char>(text), allocator);
    }
    if (j.color) {
        rapidjson::Document::ValueType color(rapidjson::kArrayType);
        color.PushBack(j.color->r, allocator);
        color.PushBack(j.color->g, allocator);
        color.PushBack(j.color->b, allocator);
        color.PushBack(j.color->a, allocator);
        v.AddMember("color", color, allocator);
    }
    if (j.fade) {
        v.AddMember("fade", *j.fade, allocator);
    }
    arr.PushBack(v, allocator);
}

void ConfigHelper::AddJSONTimeSegment(rapidjson::Document::AllocatorType& allocator, rapidjson::Document::ValueType& arr, TimeSegment& s) {
    rapidjson::Value v(rapidjson::kObjectType);
    v.AddMember("threshold", s.threshold, allocator);
    if (s.text) {
        v.AddMember("text", rapidjson::GenericStringRef<char>(s.text.value().c_str()), allocator);
    }
    arr.PushBack(v, allocator);
}

void ConfigHelper::AddJSONSegment(rapidjson::Document::AllocatorType& allocator, rapidjson::Document::ValueType& arr, Segment& s) {
    rapidjson::Value v(rapidjson::kObjectType);
    v.AddMember("threshold", s.threshold, allocator);
    if (s.text) {
        v.AddMember("text", rapidjson::GenericStringRef<char>(s.text.value().c_str()), allocator);
    }
    arr.PushBack(v, allocator);
}

void ConfigHelper::BackupConfig(ConfigDocument& config, std::string_view newPath) {
    rapidjson::StringBuffer buf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
    config.Accept(writer);
    writefile(newPath.data(), buf.GetString());
}

void ConfigHelper::RestoreConfig(std::string_view newPath) {
    writefile(Configuration::getConfigFilePath(getModInfo()).data(), readfile(newPath.data()));
    getConfig().Reload();
}

void ConfigHelper::CreateJSONSegments(rapidjson::MemoryPoolAllocator<>& allocator, ConfigDocument& config, std::vector<Segment>& vector, std::string_view name) {
    auto arr = rapidjson::Value(rapidjson::kArrayType);
    for (auto itr = vector.begin(); itr != vector.end(); ++itr) {
        ConfigHelper::AddJSONSegment(allocator, arr, *itr);
    }
    config.AddMember(rapidjson::GenericStringRef<char>(name.data()), arr, allocator);
}

void ConfigHelper::CreateJSONTimeSegments(rapidjson::MemoryPoolAllocator<>& allocator, ConfigDocument& config, std::vector<TimeSegment>& vector, std::string_view name) {
    auto arr = rapidjson::Value(rapidjson::kArrayType);
    for (auto itr = vector.begin(); itr != vector.end(); ++itr) {
        ConfigHelper::AddJSONTimeSegment(allocator, arr, *itr);
    }
    config.AddMember(rapidjson::GenericStringRef<char>(name.data()), arr, allocator);
}

bool ConfigHelper::LoadConfig(HSVConfig& con, ConfigDocument& config) {
    if (!config.IsObject()) {
        con.SetToDefault();
        return true;
    }
    static auto logger = getLogger().WithContext("ConfigHelper").WithContext("LoadConfig");
    logger.debug("got passed object check");
    // Default to true.
    // This allows us to forcibly regenerate the config if it doesn't load properly and doesn't have this property.
    con.isDefaultConfig = getBool(config, "isDefaultConfig").value_or(true);

    getJudgments(con.judgments, config);
    getSegments(con.beforeCutAngleJudgments, config, "beforeCutAngleJudgments");
    getSegments(con.accuracyJudgments, config, "accuracyJudgments");
    getSegments(con.afterCutAngleJudgments, config, "afterCutAngleJudgments");
    getTimeSegments(con.timeDependencyJudgments, config, "timeDependencyJudgments");

    // Default to standard type
    con.type = (ConfigType_t)getInt(config, "type").value_or(CONFIG_TYPE_STANDARD);

    // Default to false
    con.useFixedPos = getBool(config, "useFixedPos").value_or(false);
    // Default to 0
    con.fixedPosX = getFloat(config, "fixedPosX").value_or(0);
    con.fixedPosY = getFloat(config, "fixedPosY").value_or(0);
    con.fixedPosZ = getFloat(config, "fixedPosZ").value_or(0);

    con.timeDependencyDecimalPrecision = getInt(config, "timeDependencyDecimalPrecision").value_or(1);
    con.timeDependencyDecimalOffset = getInt(config, "timeDependencyDecimalOffset").value_or(2);
    return true;
}

void HSVConfig::WriteToConfig(ConfigDocument& config) {
    static auto logger = getLogger().WithContext("HSVConfig").WithContext("WriteToConfig");
    logger.debug("Starting to write to config");
    config.SetObject();
    config.RemoveAllMembers();
    rapidjson::MemoryPoolAllocator<>& allocator = config.GetAllocator();

    auto arr = rapidjson::Value(rapidjson::kArrayType);
    logger.debug("Starting judgments");
    logger.debug("judgments length: %lu", judgments.size());
    // Add judgments
    for (auto itr = judgments.begin(); itr != judgments.end(); ++itr) {
        logger.debug("judgment: %i", itr->threshold);
        ConfigHelper::AddJSONJudgment(allocator, arr, *itr);
    }
    config.AddMember("judgments", arr, allocator);
    // Add segments
    logger.debug("Starting segments");
    ConfigHelper::CreateJSONSegments(allocator, config, beforeCutAngleJudgments, "beforeCutAngleJudgments");
    ConfigHelper::CreateJSONSegments(allocator, config, accuracyJudgments, "accuracyJudgments");
    ConfigHelper::CreateJSONSegments(allocator, config, afterCutAngleJudgments, "afterCutAngleJudgments");
    ConfigHelper::CreateJSONTimeSegments(allocator, config, timeDependencyJudgments, "timeDependencyJudgments");
    // Set metadata
    logger.debug("Starting metadata");
    config.AddMember("type", type, allocator); // Type can be: 0: Standard, 1: Christmas, 2: Easter, etc.
    config.AddMember("useFixedPos", useFixedPos, allocator);
    config.AddMember("fixedPosX", fixedPosX, allocator);
    config.AddMember("fixedPosY", fixedPosY, allocator);
    config.AddMember("fixedPosZ", fixedPosZ, allocator);
    config.AddMember("timeDependencyDecimalPrecision", timeDependencyDecimalPrecision, allocator);
    config.AddMember("timeDependencyDecimalOffset", timeDependencyDecimalOffset, allocator);
    config.AddMember("isDefaultConfig", isDefaultConfig, allocator);
    logger.debug("Complete!");
}

void HSVConfig::SetToDefault() {
    judgments = std::vector<Judgment>(6);
    judgments[0].SetText("%BFantastic%A%n%s", {1.0f, 1.0f, 1.0f, 1.0f}, 115);
    judgments[1].SetText("<size=80%>%BExcellent%A</size>%n%s", {0.0f, 1.0f, 0.0f, 1.0f}, 101);
    judgments[2].SetText("<size=80%>%BGreat%A</size>%n%s", {1.0f, 0.980392158f, 0.0f, 1.0f}, 90);
    judgments[3].SetText("<size=80%>%BGood%A</size>%n%s", {1.0f, 0.6f, 0.0f, 1.0f}, 80, true);
    judgments[4].SetText("<size=80%>%BDecent%A</size>%n%s", {1.0f, 0.0f, 0.0f, 1.0f}, 60, true);
    judgments[5].SetText("<size=80%>%BWay Off%A</size>%n%s", {0.5f, 0.0f, 0.0f, 1.0f}, 0, true);
    beforeCutAngleJudgments = std::vector<Segment>(2);
    beforeCutAngleJudgments[0].SetText("+", 70);
    beforeCutAngleJudgments[1].SetText(" ");
    accuracyJudgments = std::vector<Segment>(2);
    accuracyJudgments[0].SetText("+", 15);
    accuracyJudgments[1].SetText(" ");
    afterCutAngleJudgments = std::vector<Segment>(2);
    afterCutAngleJudgments[0].SetText("+", 30);
    afterCutAngleJudgments[1].SetText(" ");
    timeDependencyJudgments = std::vector<TimeSegment>(0);
    useFixedPos = false;
    fixedPosX = 0;
    fixedPosY = 0;
    fixedPosZ = 0;
    timeDependencyDecimalPrecision = 1;
    timeDependencyDecimalOffset = 2;
    type = CONFIG_TYPE_STANDARD;
    isDefaultConfig = true;
}