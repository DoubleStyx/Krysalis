// Utils.h
#pragma once
#define NOMINMAX
#include <filament/Engine.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <utils/Path.h>
#include <fstream>
#include <vector>
#include <mutex>
#include "KrysalisNative.h"
#include "Utils.h"
#include "SceneBuilder.h"

using namespace filament;
using namespace math;
using namespace utils;
using namespace filamesh;

typedef void (*LogCallback)(const char* message);

extern std::mutex logMutex;
extern std::ofstream logFile;
extern std::wstring _dllDirectory;
extern LogCallback logCallback;

void startRenderingThread();
void registerLogCallback(LogCallback callback);
extern "C" __declspec(dllexport) void openLogFile();
extern "C" __declspec(dllexport) void closeLogFile();

void GlobalLog(const std::string& message);
void NativeLog(const std::string& message);
void ManagedLog(const std::string& message);

// There's a lot of stuff here. Maybe an easier way?
std::wstring getDllDirectory();
std::wstring getFullPath(const std::wstring& relativePath);
std::string wstringToString(const std::wstring& wstr);
std::wstring stringToWstring(const std::string& str);
std::string getFormattedTimestamp(std::time_t time);
