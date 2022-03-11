#pragma once

#include "json/Config.hpp"

#include "modloader/shared/modloader.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

#include "GlobalNamespace/FlyingScoreEffect.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounter.hpp"
#include "GlobalNamespace/IReadonlyCutScoreBuffer.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"

#include "HMUI/ViewController.hpp"

#define LOG_INFO(...) getLogger().info(__VA_ARGS__)
#define LOG_ERROR(...) getLogger().error(__VA_ARGS__)

Logger& getLogger();

std::string GlobalConfigPath();
std::string ConfigsPath();

extern HSV::GlobalConfig globalConfig;

bool LoadCurrentConfig();
void Judge(GlobalNamespace::CutScoreBuffer* cutScoreBuffer, GlobalNamespace::FlyingScoreEffect* flyingScoreEffect, GlobalNamespace::NoteCutInfo noteCutInfo);
