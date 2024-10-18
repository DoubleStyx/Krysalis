// KrysalisNative.h
#include <iostream>
#include <string>
#include <thread>
#include <GLFW/glfw3.h>
#include <filament/Engine.h>

typedef void (*LogCallback)(const char* message);

extern "C" __declspec(dllexport) void startRenderingThread();
extern "C" __declspec(dllexport) void RegisterLogCallback(LogCallback callback);
extern "C" __declspec(dllexport) void TestLogger(const char* msg);
