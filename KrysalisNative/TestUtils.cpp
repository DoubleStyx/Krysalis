// TestUtils.cpp
// Unit test utilities for the rendering thread
#include <thread>
#include <chrono>
#include <cassert>
#include "KrysalisNative.h"
#include "Utils.h"
#include "SceneBuilder.h"

void testRenderingThread() {
    openLogFile();

    startRenderingThread();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    closeWindow(nullptr, "Ending unit test");
}