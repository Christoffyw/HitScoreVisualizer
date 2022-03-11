#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

class JSONException : public std::exception {
    private:
        const char* error;
        std::string message;
    public:
        explicit JSONException(const std::string& message) : message(message) {
            error = message.c_str();
        }
        const char* what() const noexcept override {
            return error;
        }
};

class JSONClass {
    public:
        virtual void Deserialize(const rapidjson::Value& jsonValue) = 0;
        virtual rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) = 0;
};

#define DECLARE_JSON_CLASS(namespaze, name, impl) \
namespace namespaze { \
    class name : public JSONClass { \
        public: \
            void Deserialize(const rapidjson::Value& jsonValue); \
            rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator); \
            impl \
    }; \
}

#define DESERIALIZE_METHOD(namespaze, name, ...) \
void namespaze::name::Deserialize(const rapidjson::Value& jsonValue) { \
    __VA_ARGS__ \
}

#define DESERIALIZE_VALUE(name, jsonName, type) \
if (!jsonValue.HasMember(#jsonName)) throw JSONException(#jsonName " not found"); \
if (!jsonValue[#jsonName].Is##type()) throw JSONException(#jsonName ", type expected was: " #type); \
name = jsonValue[#jsonName].Get##type();

#define DESERIALIZE_VALUE_OPTIONAL(name, jsonName, type) \
if(jsonValue.HasMember(#jsonName) && jsonValue[#jsonName].Is##type()) { \
    name = jsonValue[#jsonName].Get##type(); \
} else name = std::nullopt;

#define DESERIALIZE_VALUE_DEFAULT(name, jsonName, type, def) \
if(jsonValue.HasMember(#jsonName) && jsonValue[#jsonName].Is##type()) { \
    name = jsonValue[#jsonName].Get##type(); \
} else name = def;

#define DESERIALIZE_CLASS(name, jsonName) \
if (!jsonValue.HasMember(#jsonName)) throw JSONException(#jsonName " not found"); \
if (!jsonValue[#jsonName].IsObject()) throw JSONException(#jsonName ", type expected was: JsonObject"); \
name.Deserialize(jsonValue[#jsonName]);

#define DESERIALIZE_CLASS_OPTIONAL(name, jsonName) \
if(jsonValue.HasMember(#jsonName) && jsonValue[#jsonName].IsObject()) { \
    if(!name.has_value()) name.emplace(); \
    name->Deserialize(jsonValue[#jsonName]); \
} else name = std::nullopt;

#define DESERIALIZE_CLASS_DEFAULT(name, jsonName, def) \
if(jsonValue.HasMember(#jsonName) && jsonValue[#jsonName].IsObject()) { \
    name.Deserialize(jsonValue[#jsonName]); \
} else name = def;

// seems to assume vector is of another json class
#define DESERIALIZE_VECTOR(name, jsonName, type) \
if (!jsonValue.HasMember(#jsonName)) throw JSONException(#jsonName " not found"); \
name.clear(); \
auto& jsonName = jsonValue[#jsonName]; \
if(jsonName.IsArray()) { \
    for (auto it = jsonName.Begin(); it != jsonName.End(); ++it) { \
        type value{}; \
        value.Deserialize(*it); \
        name.push_back(value); \
    } \
} else throw JSONException(#jsonName ", type expected was: JsonArray");

#define DESERIALIZE_VECTOR_OPTIONAL(name, jsonName, type) \
if(jsonValue.HasMember(#jsonName) && jsonValue[#jsonName].IsArray()) { \
    if(!name.has_value()) name.emplace(); \
    else name->clear(); \
    auto& jsonName = jsonValue[#jsonName]; \
    for (auto it = jsonName.Begin(); it != jsonName.End(); ++it) { \
        type value{}; \
        value.Deserialize(*it); \
        name->push_back(value); \
    } \
} else name = std::nullopt;

#define DESERIALIZE_VECTOR_DEFAULT(name, jsonName, type, def) \
if(jsonValue.HasMember(#jsonName) && jsonValue[#jsonName].IsArray()) { \
    name.clear(); \
    auto& jsonName = jsonValue[#jsonName]; \
    for (auto it = jsonName.Begin(); it != jsonName.End(); ++it) { \
        type value{}; \
        value.Deserialize(*it); \
        name.push_back(value); \
    } \
} else name = def;

#define DESERIALIZE_VECTOR_BASIC(name, jsonName, type) \
if (!jsonValue.HasMember(#jsonName)) throw JSONException(#jsonName " not found"); \
name.clear(); \
auto& jsonName = jsonValue[#jsonName]; \
if(jsonName.IsArray()) { \
    for (auto it = jsonName.Begin(); it != jsonName.End(); ++it) { \
        name.push_back(it->Get##type()); \
    } \
} else throw JSONException(#jsonName ", type expected was: JsonArray");

#define DESERIALIZE_VECTOR_BASIC_OPTIONAL(name, jsonName, type) \
if(jsonValue.HasMember(#jsonName) && jsonValue[#jsonName].IsArray()) { \
    if(!name.has_value()) name.emplace(); \
    else name->clear(); \
    auto& jsonName = jsonValue[#jsonName]; \
    for (auto it = jsonName.Begin(); it != jsonName.End(); ++it) { \
        name->push_back(it->Get##type()); \
    } \
} else name = std::nullopt;

#define DESERIALIZE_VECTOR_BASIC_DEFAULT(name, jsonName, type, def) \
if(jsonValue.HasMember(#jsonName) && jsonValue[#jsonName].IsArray()) { \
    name.clear(); \
    auto& jsonName = jsonValue[#jsonName]; \
    for (auto it = jsonName.Begin(); it != jsonName.End(); ++it) { \
        name.push_back(it->Get##type()); \
    } \
} else name = def;

#define SERIALIZE_METHOD(namespaze, name, ...) \
rapidjson::Value namespaze::name::Serialize(rapidjson::Document::AllocatorType& allocator) { \
    rapidjson::Value jsonObject(rapidjson::kObjectType); \
    __VA_ARGS__ \
    return jsonObject; \
}

#define SERIALIZE_VALUE(name, jsonName) \
jsonObject.AddMember(#jsonName, name, allocator);

#define SERIALIZE_VALUE_OPTIONAL(name, jsonName) \
if(name) jsonObject.AddMember(#jsonName, name.value(), allocator);

#define SERIALIZE_CLASS(name, jsonName) \
jsonObject.AddMember(#jsonName, name.Serialize(allocator), allocator);

#define SERIALIZE_CLASS_OPTIONAL(name, jsonName) \
if(name) jsonObject.AddMember(#jsonName, name->Serialize(allocator), allocator);

// assumes vector is of json serializables
#define SERIALIZE_VECTOR(name, jsonName) \
rapidjson::Value name##_jsonArray(rapidjson::kArrayType); \
for(auto jsonClass : name) { \
    name##_jsonArray.GetArray().PushBack(jsonClass.Serialize(allocator), allocator); \
} \
jsonObject.AddMember(#jsonName, name##_jsonArray, allocator);

#define SERIALIZE_VECTOR_OPTIONAL(name, jsonName) \
if(name) { \
    rapidjson::Value name##_jsonArray(rapidjson::kArrayType); \
    for(auto jsonClass : name.value()) { \
        name##_jsonArray.GetArray().PushBack(jsonClass.Serialize(allocator), allocator); \
    } \
    jsonObject.AddMember(#jsonName, name##_jsonArray, allocator);\
}

#define SERIALIZE_VECTOR_BASIC(name, jsonName) \
rapidjson::Value name##_jsonArray(rapidjson::kArrayType); \
for(auto member : name) { \
    name##_jsonArray.GetArray().PushBack(rapidjson::Value(member, allocator).Move(), allocator); \
} \
jsonObject.AddMember(#jsonName, name##_jsonArray, allocator);

#define SERIALIZE_VECTOR_BASIC_OPTIONAL(name, jsonName) \
if(name) { \
    rapidjson::Value name##_jsonArray(rapidjson::kArrayType); \
    for(auto member : name.value()) { \
        name##_jsonArray.GetArray().PushBack(rapidjson::Value(member, allocator).Move(), allocator); \
    } \
    jsonObject.AddMember(#jsonName, name##_jsonArray, allocator); \
}

// functions, will be included with class definitions
static void ReadFromFile(std::string_view path, JSONClass& toDeserialize) {
    if(!fileexists(path))
        throw JSONException("file not found");
    auto json = readfile(path);

    rapidjson::Document document;
    document.Parse(json);
    if(document.HasParseError() || !document.IsObject())
        throw JSONException("file could not be parsed as json");
    
    toDeserialize.Deserialize(document.GetObject());
}

static bool WriteToFile(std::string_view path, JSONClass& toSerialize) {
    rapidjson::Document document;
    document.SetObject();
    toSerialize.Serialize(document.GetAllocator()).Swap(document);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    std::string s = buffer.GetString();

    return writefile(path, s);
}