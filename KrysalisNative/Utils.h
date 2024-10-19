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

std::vector<uint8_t> loadFile(const std::wstring& relativePath);

filament::Texture* loadTexture(filament::Engine* engine, const std::wstring& relativePath);

typedef void (*LogCallback)(const char* message);

extern "C" __declspec(dllexport) void startRenderingThread();
extern "C" __declspec(dllexport) void RegisterLogCallback(LogCallback callback);
extern "C" __declspec(dllexport) void TestLogger(const char* msg);

void LogToCSharp(const std::string& message);
std::wstring getDllDirectory();
std::wstring getFullPath(const std::wstring& relativePath);
std::string wstringToString(const std::wstring& wstr);
filament::IndirectLight* createIBL(filament::Engine* engine, const std::wstring& envMapPath);