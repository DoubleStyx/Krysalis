// KrysalisNative.cpp
#include <iostream>
#include <filesystem>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <vector>
#include <GLFW/glfw3.h>
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
    LogToCSharp("Vertex buffer created");

    vb->setBufferAt(*engine, 0, filament::VertexBuffer::BufferDescriptor(vertices, sizeof(vertices)));
    LogToCSharp("Vertex buffer assigned");

    static const uint16_t indices[3] = { 0, 1, 2 };

    filament::IndexBuffer* ib = filament::IndexBuffer::Builder()
        .indexCount(3)
        .bufferType(filament::IndexBuffer::IndexType::USHORT)
        .build(*engine);
    LogToCSharp("Index buffer created");

    ib->setBuffer(*engine, filament::IndexBuffer::BufferDescriptor(indices, sizeof(indices)));
    LogToCSharp("Index buffer assigned");

    char path[MAX_PATH];

    HMODULE hModule = nullptr;
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR)&LogToCSharp, &hModule);
    LogToCSharp("Module handle acquired");

    GetModuleFileNameA(hModule, path, MAX_PATH);

    LogToCSharp("Module file name acquired");

    std::string dllDir = std::string(path);
    LogToCSharp("Path found: " + dllDir);

    std::string execDir = dllDir.substr(0, dllDir.find_last_of("\\/"));
    std::string materialPath = execDir + "\\assets\\materials\\sandboxUnlit.filamat";
    LogToCSharp("Material path: " + materialPath);

    std::ifstream matFile(materialPath, std::ios::binary | std::ios::ate);
    LogToCSharp("Started input stream for mat file");
    std::streamsize size = matFile.tellg();
    matFile.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (matFile.read(buffer.data(), size)) {
        LogToCSharp("Read material file");
        filament::Material* material = filament::Material::Builder()
            .package(buffer.data(), buffer.size())
            .build(*engine);

        LogToCSharp("Material created");

        filament::MaterialInstance* materialInstance = material->createInstance();
        LogToCSharp("Material instantiated");

        // Set the base color and emissive properties on the material instance
        filament::math::float3 baseColor = { 1.0f, 0.0f, 0.0f }; // Red color
        filament::math::float4 emissive = { 0.0f, 0.0f, 0.0f, 1.0f }; // No emissive

        materialInstance->setParameter("baseColor", baseColor);
        materialInstance->setParameter("emissive", emissive);
        LogToCSharp("Material parameters set");

        utils::Entity triangleEntity = utils::EntityManager::get().create();
        LogToCSharp("Triangle entity created");

        filament::RenderableManager::Builder(1)
            .boundingBox({ {0, 0, 0}, {1, 1, 1} })
            .geometry(0, filament::RenderableManager::PrimitiveType::TRIANGLES, vb, ib)
            .material(0, materialInstance)
            .build(*engine, triangleEntity);

        scene->addEntity(triangleEntity);
        LogToCSharp("Triangle entity added to scene");
    }
}

void runWindow() {
    try
    {
        if (!glfwInit()) {
            LogToCSharp("Failed to initialize GLFW.");
            return;
        }
        LogToCSharp("GLFW initialized");

        window = glfwCreateWindow(800, 600, "My Filament Window", nullptr, nullptr);
        if (!window) {
            LogToCSharp("Failed to create window.");
            glfwTerminate();
            return;
        }
        LogToCSharp("Window created");

        glfwMakeContextCurrent(window);
        LogToCSharp("Set current window as glfw context");
        glfwSwapInterval(1);
        LogToCSharp("Set glfw swap interval");

        engine = filament::Engine::create();
        if (!engine) {
            LogToCSharp("Failed to create Filament engine.");
            glfwDestroyWindow(window);
            glfwTerminate();
            return;
        }
        LogToCSharp("Created filament engine instance");

        swapChain = engine->createSwapChain(nullptr);
        LogToCSharp("Swap chain created");

        renderer = engine->createRenderer();
        LogToCSharp("Renderer created");

        scene = engine->createScene();
        view = engine->createView();
        LogToCSharp("Scene and view created");

        camera = engine->createCamera(utils::EntityManager::get().create());
        view->setCamera(camera);
        view->setScene(scene);
        LogToCSharp("Camera initialized");

        filament::Viewport viewport(0, 0, 800, 600);
        view->setViewport(viewport);
        LogToCSharp("Viewport created");

        createTriangle(engine);
        LogToCSharp("Triangle created");

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (renderer->beginFrame(swapChain)) {
                renderer->render(view);
                renderer->endFrame();
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        LogToCSharp("Closing renderer and freeing memory");
        filament::Engine::destroy(engine);
        glfwDestroyWindow(window);
        glfwTerminate();
        LogToCSharp("Renderer closed");
    }
    catch (const std::exception& e)
    {
        LogToCSharp("Error in rendering thread: " + std::string(e.what()));
    }
    catch (...) {
        LogToCSharp("Error in rendering thread: Caught unknown exception");
    }
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
