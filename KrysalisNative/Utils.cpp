// Utils.cpp
// Miscellaneous utility/helper functions

#include <filament/Engine.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/Texture.h>
#include <utils/Path.h>
#include <fstream>
#include <vector>
#include <stb_image.h>
#include <iostream>
#include <sstream>
#include <string>
#include <mutex>
#include <Windows.h>
#include <Shlwapi.h>
#include <ktxreader/Ktx2Reader.h>
#include <filament/IndirectLight.h>
#include <locale>
#include <iomanip>
#include <codecvt>
#include "KrysalisNative.h"
#include "Utils.h"
#include "SceneBuilder.h"

using namespace filament;
using namespace math;

std::mutex logMutex;
std::ofstream logFile;
std::wstring _dllDirectory;
LogCallback logCallback = nullptr;

std::wstring getFullPath(const std::wstring& relativePath) {
	if (_dllDirectory.empty()) {
		_dllDirectory = getDllDirectory();
	}
	return _dllDirectory + L"\\" + relativePath;
}

std::string wstringToString(const std::wstring& wstr) {
    return std::string(wstr.begin(), wstr.end());
}

std::wstring stringToWstring(const std::string& str) {
    return std::wstring(str.begin(), str.end());
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

extern "C" __declspec(dllexport) void registerLogCallback(LogCallback callback)
{
    logCallback = callback;
}

void ManagedLog(const std::string& message)
{
    if (logCallback)
    {
        logCallback(message.c_str());
    }
}

void NativeLog(const std::string& message) {
    std::lock_guard<std::mutex> guard(logMutex);

    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);

    std::tm localTime;
    localtime_s(&localTime, &now_time);

    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::ostringstream timestampStream;
    timestampStream << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    timestampStream << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();

    std::string timestamp = timestampStream.str();

    if (logFile.is_open()) {
        logFile << "[" << timestamp << "] " << message << std::endl;
    }
    else {
        std::cerr << "Log file is not open, cannot write log" << std::endl;
    }
}

void GlobalLog(const std::string& message) {
	NativeLog(message);
	ManagedLog(message);
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

void openLogFile() {
    auto currentTime = std::chrono::system_clock::now();

    std::time_t now_time = std::chrono::system_clock::to_time_t(currentTime);
    std::string timestamp = getFormattedTimestamp(now_time);

    std::wstring relativePath = stringToWstring("logs\\" + timestamp + ".log");
    logFile.open(getFullPath(relativePath), std::ios::out | std::ios::app);
}

void closeLogFile() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

std::string getFormattedTimestamp(std::time_t time) {
    std::tm localTime;
    localtime_s(&localTime, &time);

    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d_%H-%M-%S");

    return oss.str();
}