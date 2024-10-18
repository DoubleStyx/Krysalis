// KrysalisNative.cpp
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <GLFW/glfw3.h>
#include <filament/Engine.h>

// Globals for the Filament engine
filament::Engine* engine = nullptr;
GLFWwindow* window = nullptr;
std::mutex logMutex;  // Mutex for thread-safe logging

// Function to initialize the window and run the rendering loop in a separate thread
void runWindow() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(800, 600, "My Filament Window", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window." << std::endl;
        glfwTerminate();
        return;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Set the swap interval for VSync
    glfwSwapInterval(1);  // 1 to enable VSync, 0 to disable

    // Initialize Filament engine
    engine = filament::Engine::create();
    if (!engine) {
        std::cerr << "Failed to create Filament engine." << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return;
    }

    // Rendering loop
    while (!glfwWindowShouldClose(window)) {
        // Update rendering here (you can pass data from the main thread)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear buffers

        // Add your rendering logic here (when needed)

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Cleanup Filament and GLFW
    filament::Engine::destroy(engine);  // Correct destruction of the engine
    glfwDestroyWindow(window);
    glfwTerminate();
}

// Function to start the rendering thread
extern "C" __declspec(dllexport) void startRenderingThread() {
    std::thread renderThread(runWindow);  // Launch the rendering thread
    renderThread.detach();  // Detach the thread so it runs independently
}

// Define a function pointer for the C# callback
typedef void (*LogCallback)(const char* message);

// Global variable to store the callback
LogCallback logCallback = nullptr;

// Function to register the log callback
extern "C" __declspec(dllexport) void RegisterLogCallback(LogCallback callback) {
    std::lock_guard<std::mutex> lock(logMutex);
    logCallback = callback;
}

// Function to log messages
void LogToCSharp(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logCallback) {
        logCallback(message.c_str());
    }
}

// Test logger function now accepts a C-style string (const char*)
extern "C" __declspec(dllexport) void TestLogger(const char* msg) {
    LogToCSharp(msg);  // Pass the message to the logger
}
