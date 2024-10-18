// KrysalisNative.cpp
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <GLFW/glfw3.h>
#include <filament/Engine.h>
#include "KrysalisNative.h"

filament::Engine* engine = nullptr;
GLFWwindow* window = nullptr;
std::mutex logMutex;

void runWindow() {
    if (!glfwInit()) {
        LogToCSharp("Failed to initialize GLFW.");
        return;
    }

    window = glfwCreateWindow(800, 600, "My Filament Window", nullptr, nullptr);
    if (!window) {
        LogToCSharp("Failed to create window.");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    engine = filament::Engine::create();
    if (!engine) {
        LogToCSharp("Failed to create Filament engine.");
        glfwDestroyWindow(window);
        glfwTerminate();
        return;
    }

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    filament::Engine::destroy(engine);
    glfwDestroyWindow(window);
    glfwTerminate();
}

extern "C" __declspec(dllexport) void startRenderingThread() {
    std::thread renderThread(runWindow);
    renderThread.detach();
}

typedef void (*LogCallback)(const char* message);

LogCallback logCallback = nullptr;

extern "C" __declspec(dllexport) void RegisterLogCallback(LogCallback callback) {
    std::lock_guard<std::mutex> lock(logMutex);
    logCallback = callback;
}

void LogToCSharp(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logCallback) {
        logCallback(message.c_str());
    }
}

extern "C" __declspec(dllexport) void TestLogger(const char* msg) {
    LogToCSharp(msg);
}
