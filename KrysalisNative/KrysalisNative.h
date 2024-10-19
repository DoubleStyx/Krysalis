// KrysalisNative.h

// Type definitions
typedef void (*LogCallback)(const char* message);

// Interop declarations
extern "C" __declspec(dllexport) void startRenderingThread();
extern "C" __declspec(dllexport) void RegisterLogCallback(LogCallback callback);
extern "C" __declspec(dllexport) void TestLogger(const char* msg);

// Native declarations
void LogToCSharp(const std::string& message);
void closeWindow(GLFWwindow* window, std::string reason);