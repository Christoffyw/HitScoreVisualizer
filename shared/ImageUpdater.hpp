#pragma once
#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
using namespace UnityEngine;

DECLARE_CLASS_CODEGEN(TestMod, ImageUpdater, UnityEngine::MonoBehaviour,

    DECLARE_INSTANCE_METHOD(void, Update);

)