// Utils.h
#pragma once
#include <filament/Engine.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <utils/Path.h>
#include <fstream>
#include <vector>
#include "KrysalisNative.h"
#include "Utils.h"

using namespace filament;
using namespace math;

typedef void (*LogCallback)(const char* message);

extern std::mutex logMutex;
extern std::ofstream logFile;
extern std::wstring _dllDirectory;
extern LogCallback logCallback;

extern "C" __declspec(dllexport) void startRenderingThread();
extern "C" __declspec(dllexport) void registerLogCallback(LogCallback callback);
extern "C" __declspec(dllexport) void TestLogger(const char* msg);
extern "C" __declspec(dllexport) void openLogFile();

void GlobalLog(const std::string& message);
void NativeLog(const std::string& message);
void ManagedLog(const std::string& message);
std::wstring getDllDirectory();
std::wstring getFullPath(const std::wstring& relativePath);
std::string wstringToString(const std::wstring& wstr);
std::wstring stringToWstring(const std::string& str);
std::string getFormattedTimestamp(std::time_t time);
std::vector<uint8_t> loadFile(const std::wstring& relativePath);
filament::Texture* loadTexture(filament::Engine* engine, const std::wstring& relativePath);
void closeLogFile();