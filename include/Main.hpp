#pragma once

#include "GlobalNamespace/CutScoreBuffer.hpp"
#include "GlobalNamespace/FlyingScoreEffect.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "json/Config.hpp"

constexpr auto logger = Paper::ConstLoggerContext(MOD_ID);

std::string GlobalConfigPath();
std::string ConfigsPath();

extern HSV::GlobalConfig globalConfig;

bool LoadCurrentConfig();
void Judge(
    GlobalNamespace::CutScoreBuffer* cutScoreBuffer, GlobalNamespace::FlyingScoreEffect* flyingScoreEffect, GlobalNamespace::NoteCutInfo& noteCutInfo
);
