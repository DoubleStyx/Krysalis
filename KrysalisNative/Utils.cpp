// Utils.cpp
#include <filament/Engine.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/Texture.h>
#include <utils/Path.h>
#include <fstream>
#include <vector>
#include <stb_image.h>
#include <iostream>
#include <string>
#include <mutex>
#include <Windows.h>
#include <Shlwapi.h>
#include "KrysalisNative.h"
#include "Utils.h"

std::wstring getFullPath(const std::wstring& relativePath) {
	if (_dllDirectory.empty()) {
		_dllDirectory = getDllDirectory();
	}
	return _dllDirectory + L"\\" + relativePath;
}

std::string wstringToString(const std::wstring& wstr) {
    return std::string(wstr.begin(), wstr.end());
}

std::vector<uint8_t> loadFile(const std::wstring& relativePath) {
    std::ifstream file(getFullPath(relativePath), std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open file: " + wstringToString(relativePath));
    }
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});
}

filament::Texture* loadTexture(filament::Engine* engine, const std::wstring& relativePath) {
    std::vector<uint8_t> fileData = loadFile(relativePath);

    int width, height, channels;
    unsigned char* data = stbi_load_from_memory(fileData.data(), fileData.size(), &width, &height, &channels, 4);

    if (!data) {
        throw std::runtime_error("Failed to load image: " + wstringToString(relativePath));
    }

    filament::Texture* texture = filament::Texture::Builder()
        .width(width)
        .height(height)
        .levels(1)
        .format(filament::Texture::InternalFormat::RGBA8)
        .build(*engine);

    filament::Texture::PixelBufferDescriptor buffer(data, size_t(width * height * 4),
        filament::Texture::Format::RGBA, filament::Texture::Type::UBYTE,
        [](void* buffer, size_t, void*) { stbi_image_free(buffer); });

    texture->setImage(*engine, 0, std::move(buffer));

    return texture;
}


extern "C" __declspec(dllexport) void startRenderingThread()
{
    std::thread renderThread(runWindow);
    renderThread.detach();
}

typedef void (*LogCallback)(const char* message);

LogCallback logCallback = nullptr;

extern "C" __declspec(dllexport) void RegisterLogCallback(LogCallback callback)
{
    std::lock_guard<std::mutex> lock(logMutex);
    logCallback = callback;
}

void LogToCSharp(const std::string& message)
{
    std::lock_guard<std::mutex> lock(logMutex);
    if (logCallback)
    {
        logCallback(message.c_str());
    }
}

extern "C" __declspec(dllexport) void TestLogger(const char* msg)
{
    LogToCSharp(msg);
}

std::wstring getDllDirectory() {
    WCHAR path[MAX_PATH];

    HMODULE hModule = NULL;
    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)getDllDirectory,
        &hModule)) {
        GetModuleFileName(hModule, path, sizeof(path) / sizeof(WCHAR));

        PathRemoveFileSpec(path);

        return std::wstring(path);
    }

    return std::wstring();
}