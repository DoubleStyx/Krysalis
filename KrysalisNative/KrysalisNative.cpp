// KrysalisNative.cpp
#define GLFW_EXPOSE_NATIVE_WIN32
#include <iostream>
#include <filesystem>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <vector>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <filament/Engine.h>
#include <filament/Scene.h>
#include <filament/Renderer.h>
#include <filament/View.h>
#include <filament/Camera.h>
#include <filament/SwapChain.h>
#include <filament/VertexBuffer.h>
#include <filament/IndexBuffer.h>
#include <filament/Material.h>
#include <filament/RenderableManager.h>
#include <utils/EntityManager.h>
#include <filament/Viewport.h>
#include <Windows.h>
#include <GL/gl.h>
#include "KrysalisNative.h"

filament::Engine* engine = nullptr;
GLFWwindow* window = nullptr;
filament::Renderer* renderer = nullptr;
filament::View* view = nullptr;
filament::Scene* scene = nullptr;
filament::Camera* camera = nullptr;
filament::SwapChain* swapChain = nullptr;
std::mutex logMutex;

void createTriangle(filament::Engine* engine) {
    static const filament::math::float3 vertices[3] = {
        { 0.0f,  0.5f, 0.0f },
        { 0.5f, -0.5f, 0.0f },
        {-0.5f, -0.5f, 0.0f }
    };

    filament::VertexBuffer* vb = filament::VertexBuffer::Builder()
        .vertexCount(3)
        .bufferCount(1)
        .attribute(filament::VertexAttribute::POSITION, 0, filament::VertexBuffer::AttributeType::FLOAT3)
        .build(*engine);
    if (vb == nullptr)
        closeWindow("Vertex buffer failed to initialize");
    LogToCSharp("Vertex buffer created");

    vb->setBufferAt(*engine, 0, filament::VertexBuffer::BufferDescriptor(vertices, sizeof(vertices)));
    LogToCSharp("Vertex buffer assigned");

    static const uint16_t indices[3] = { 0, 1, 2 };

    filament::IndexBuffer* ib = filament::IndexBuffer::Builder()
        .indexCount(3)
        .bufferType(filament::IndexBuffer::IndexType::USHORT)
        .build(*engine);
    if (ib == nullptr)
        closeWindow("Index buffer failed to initialize");
    LogToCSharp("Index buffer created");

    ib->setBuffer(*engine, filament::IndexBuffer::BufferDescriptor(indices, sizeof(indices)));
    LogToCSharp("Index buffer assigned");

    filament::MaterialInstance* materialInstance = engine->getDefaultMaterial()->createInstance();
    if (materialInstance == nullptr)
        closeWindow("Material instance not initialized");
    LogToCSharp("Material instantiated");

    utils::Entity triangleEntity = utils::EntityManager::get().create();
    if (triangleEntity.isNull())
        closeWindow("Triangle entity not initialized");
    LogToCSharp("Triangle entity created");

    filament::RenderableManager::Builder(1)
        .boundingBox({ {0, 0, 0}, {1, 1, 1} })
        .geometry(0, filament::RenderableManager::PrimitiveType::TRIANGLES, vb, ib)
        .material(0, materialInstance)
        .build(*engine, triangleEntity);
    LogToCSharp("Added renderable to triangle entity");

    scene->addEntity(triangleEntity);
    LogToCSharp("Triangle entity added to scene");
}

void runWindow() {
    try {
        glfwInit();
        LogToCSharp("GLFW initialized");

        // Set the desired OpenGL version and profile
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // Optional: Make the window not resizable
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(800, 600, "Krysalis", nullptr, nullptr);
        if (window == nullptr)
            closeWindow("Window not initialized");
        LogToCSharp("Window created");

        // Make the context current
        glfwMakeContextCurrent(window);
        LogToCSharp("Set current window as GLFW context");

        // Retrieve and log the OpenGL version
        const GLubyte* version = glGetString(GL_VERSION);
        if (version == NULL) {
            closeWindow("Failed to retrieve OpenGL version");
        }
        LogToCSharp("OpenGL Version: " + std::string(reinterpret_cast<const char*>(version)));


        // After logging the OpenGL version, verify it
        std::string glVersionStr(reinterpret_cast<const char*>(version));
        int major, minor;
        sscanf(glVersionStr.c_str(), "%d.%d", &major, &minor);
        if (major < 4 || (major == 4 && minor < 1)) {
            closeWindow("Insufficient OpenGL version. Required: 4.1, Detected: " + glVersionStr);
        }

        glfwSwapInterval(1);
        LogToCSharp("Set glfw swap interval");

        engine = filament::Engine::create(filament::Engine::Backend::OPENGL);
        if (engine == NULL)
            closeWindow("Engine not initialized");
        LogToCSharp("Created Filament engine instance");

        // Create swap chain with native window handle
        HWND hwnd = glfwGetWin32Window(window);
        if (hwnd == NULL)
            closeWindow("Native window handle is empty");
        LogToCSharp("Obtained the native window handle.");

        HDC hdc = GetDC(hwnd);
        if (hdc == NULL)
            closeWindow("Device context (HDC) is empty");
        LogToCSharp("Obtained the device context (HDC).");

        swapChain = engine->createSwapChain((void*)hwnd);
        if (swapChain == NULL)
            closeWindow("Swap chain not initialized");
        LogToCSharp("Swap chain created with native window handle");

        renderer = engine->createRenderer();
        if (renderer == NULL)
            closeWindow("Renderer not initialized");
        LogToCSharp("Renderer created");

        scene = engine->createScene();
        if (scene == NULL)
            closeWindow("Scene not initialized");
        LogToCSharp("Scene created");
        view = engine->createView();
        if (view == NULL)
            closeWindow("View not initialized");
        LogToCSharp("View created");

        camera = engine->createCamera(utils::EntityManager::get().create());
        if (camera == NULL)
            closeWindow("Camera not initialized");
        LogToCSharp("Camera created");
        view->setCamera(camera);
        LogToCSharp("Camera assigned to view");
        view->setScene(scene);
        LogToCSharp("Scene assigned to view");

        filament::Viewport viewport(0, 0, 800, 600);
        view->setViewport(viewport);
        LogToCSharp("Viewport created");

        createTriangle(engine);
        LogToCSharp("Triangle created");

        LogToCSharp("Beginning main rendering loop");
        CheckOpenGLErrors("Start of main rendering loop");
        while (!glfwWindowShouldClose(window)) {
            LogToCSharp("Checking if next frame is ready");
            bool frameStarted = renderer->beginFrame(swapChain);
            if (frameStarted) {
                CheckOpenGLErrors("Start of new frame");
                LogToCSharp("Beginning new frame");
                renderer->render(view);
                CheckOpenGLErrors("After rendering the view");
                LogToCSharp("Rendered view");
                renderer->endFrame();
                CheckOpenGLErrors("End of frame");
                LogToCSharp("Frame ended");
            }
            else {
                LogToCSharp("Not able to begin next frame");
            }
            glfwPollEvents();
            CheckOpenGLErrors("After event poll");
            LogToCSharp("GLFW events polled");
        }
    }
    catch (const std::exception& e)
    {
        closeWindow("Error in rendering thread: " + std::string(e.what()));
    }
    catch (...) {
        closeWindow("Error in rendering thread: Caught unknown exception");
    }
}

void CheckOpenGLErrors(const std::string context) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        closeWindow("OpenGL error in " + context + ": " + std::to_string(err));
    }
}

void closeWindow(std::string reason) {
    LogToCSharp(reason);
    LogToCSharp("Closing renderer and freeing memory");
    filament::Engine::destroy(engine);
    LogToCSharp("Closed the engine");
    glfwDestroyWindow(window);
    LogToCSharp("Destroyed the GLFW window");
    // Release the Device Context (HDC)
    HWND hwnd = glfwGetWin32Window(window);
    if (hwnd) {
        HDC hdc = GetDC(hwnd);
        if (hdc) {
            ReleaseDC(hwnd, hdc);
            LogToCSharp("Released device context (HDC)");
        }
    }
    glfwTerminate();
    LogToCSharp("GLFW terminated");
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
