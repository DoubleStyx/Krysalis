// TestUtils.h
#pragma once
#include <thread>
#include <chrono>
#include <cassert>
#include "KrysalisNative.h"
#include "Utils.h"
#include "SceneBuilder.h"

extern "C" __declspec(dllexport) void testRenderingThread();