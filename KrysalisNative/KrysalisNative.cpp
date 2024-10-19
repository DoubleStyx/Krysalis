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
#include <filament/TransformManager.h>
#include <filament/LightManager.h>
#include <filament/Material.h>
#include <filament/RenderableManager.h>
#include <utils/EntityManager.h>
#include <filament/Viewport.h>
#include <Windows.h>
#include <chrono>
#include <thread>
#include "KrysalisNative.h"

constexpr double TARGET_FPS = 60.0;
constexpr double TARGET_FRAME_DURATION = 1.0 / TARGET_FPS;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_4
#define M_PI_4 0.78539816339744830962
#endif

using namespace filament;

struct Vertex {
    math::float2 position;
    uint32_t color;
};

static const Vertex TRIANGLE_VERTICES[3] = {
    {{1.0f, 0.0f}, 0xffff0000u},
    {{cosf(M_PI * 2 / 3), sinf(M_PI * 2 / 3)}, 0xff00ff00u},
    {{cosf(M_PI * 4 / 3), sinf(M_PI * 4 / 3)}, 0xff0000ffu}
};

static constexpr uint16_t TRIANGLE_INDICES[3] = { 0, 1, 2 };

filament::Engine* _engine = nullptr;
filament::SwapChain* _swapchain = nullptr;
filament::Renderer* _renderer = nullptr;
filament::Camera* _camera = nullptr;
filament::View* _view = nullptr;
filament::Scene* _scene = nullptr;
utils::Entity _entity;
std::mutex logMutex;

int g_window_size_x = 512;
int g_window_size_y = 512;
int g_frame_size_x = g_window_size_x;
int g_frame_size_y = g_window_size_y;

float rotationAngle = 0.0f;

void* getNativeWindow(GLFWwindow* window) {
    LogToCSharp("Getting native window");
    HWND hwnd = glfwGetWin32Window(window);
    if (hwnd == nullptr) {
        closeWindow(nullptr, "Native window not found");
    }
    LogToCSharp("Got native window");
    return hwnd;
}

void reshape_window(GLFWwindow* window, int w, int h) {
    LogToCSharp("Reshaping window");
    (void)window; (void)w; (void)h;
    LogToCSharp("Reshaped window");
}

void reshape_framebuffer(GLFWwindow* window, int w, int h) {
    (void)window; (void)w; (void)h;
}

void key_press(GLFWwindow* window, int key, int scancode, int action, int mods) {
    LogToCSharp("Key pressed");
    (void)window; (void)mods; (void)scancode;
    LogToCSharp("Key: " + std::to_string(key) + " Action: " + std::to_string(action));
    if (action != GLFW_PRESS) {
        LogToCSharp("Action not pressed");
        return;
    }

    if (key == GLFW_KEY_Q) {
        LogToCSharp("Key Q pressed");
        exit(0);
    }

    LogToCSharp("Key not Q");
}

void addLight(filament::Engine* engine, filament::Scene* scene) {
    filament::LightManager& lm = engine->getLightManager();
    LogToCSharp("Got light manager");
    utils::Entity lightEntity = utils::EntityManager::get().create();
    if (lightEntity.isNull()) {
        closeWindow(nullptr, "Light entity not created");
    }
    LogToCSharp("Light entity created");

    LightManager::Builder(LightManager::Type::DIRECTIONAL)
        .color(Color::toLinear<ACCURATE>(sRGBColor(0.98f, 0.92f, 0.89f)))
        .intensity(110000)
        .direction({ 0.7, -1, -0.8 })
        .sunAngularRadius(1.9f)
        .castShadows(false)
        .build(*engine, lightEntity);
    LogToCSharp("Built light entity");
    scene->addEntity(lightEntity);
    LogToCSharp("Directional light added to scene");
}


void init(GLFWwindow* window) {
    filament::Engine* engine = filament::Engine::create();
    if (!engine) {
        closeWindow(nullptr, "Engine not created");
    }
    LogToCSharp("Engine created");

    filament::SwapChain* swapChain = engine->createSwapChain(getNativeWindow(window));
    if (!swapChain) {
        closeWindow(nullptr, "Swap chain not created");
    }
    LogToCSharp("Swap chain created");

    filament::Renderer* renderer = engine->createRenderer();
    if (!renderer) {
        closeWindow(nullptr, "Renderer not created");
    }
    LogToCSharp("Renderer created");

    utils::Entity cameraEntity = utils::EntityManager::get().create();
    if (cameraEntity.isNull()) {
        closeWindow(nullptr, "Camera entity not created");
    }
    LogToCSharp("Camera entity created");

    Camera* camera = engine->createCamera(cameraEntity);
    if (!camera) {
        closeWindow(nullptr, "Camera not created");
    }
    LogToCSharp("Camera created");

    filament::View* view = engine->createView();
    if (!view) {
        closeWindow(nullptr, "View not created");
    }
    LogToCSharp("View created");

    filament::Scene* scene = engine->createScene();
    if (!scene) {
        closeWindow(nullptr, "Scene not created");
    }
    LogToCSharp("Scene created");

    view->setViewport({ 0, 0, static_cast<uint32_t>(g_frame_size_x), static_cast<uint32_t>(g_frame_size_y) });
    LogToCSharp("Set viewport");

    {
        math::double3 eye(0.0, 0.0, 3.0);
        math::double3 at(0.0, 0.0, 0.0);
        math::double3 up(0.0, 1.0, 0.0);

        math::mat4 viewMatrix = math::mat4::lookAt(eye, at, up);
        camera->setModelMatrix(math::mat4f(viewMatrix));
        LogToCSharp("Set camera model matrix using lookAt");
    }

    camera->setProjection(
        45.0f,
        static_cast<float>(g_frame_size_x) / g_frame_size_y,
        0.1f,
        100.0f,
        filament::Camera::Fov::VERTICAL
    );
    LogToCSharp("Set camera projection (Perspective)");

    renderer->setClearOptions({
        .clearColor = {0.25f, 0.5f, 1.0f, 1.0f},
        .clear = true
        });
    LogToCSharp("Set clear options");

    view->setPostProcessingEnabled(false);
    LogToCSharp("Set post processing enabled");

    filament::VertexBuffer* vb = filament::VertexBuffer::Builder()
        .vertexCount(3)
        .bufferCount(1)
        .attribute(filament::VertexAttribute::POSITION, 0, filament::VertexBuffer::AttributeType::FLOAT2, offsetof(Vertex, position), sizeof(Vertex))
        .attribute(filament::VertexAttribute::COLOR, 0, filament::VertexBuffer::AttributeType::UBYTE4, offsetof(Vertex, color), sizeof(Vertex))
        .normalized(filament::VertexAttribute::COLOR)
        .build(*engine);
    if (!vb) {
        closeWindow(nullptr, "Vertex buffer not created");
    }
    LogToCSharp("Vertex buffer created");

    vb->setBufferAt(*engine, 0,
        filament::VertexBuffer::BufferDescriptor(TRIANGLE_VERTICES, sizeof(TRIANGLE_VERTICES), nullptr));
    LogToCSharp("Set vertex buffer");

    filament::IndexBuffer* ib = filament::IndexBuffer::Builder()
        .indexCount(3)
        .bufferType(filament::IndexBuffer::IndexType::USHORT)
        .build(*engine);
    if (!ib) {
        closeWindow(nullptr, "Index buffer not created");
    }
    LogToCSharp("Index buffer created");

    ib->setBuffer(*engine,
        filament::IndexBuffer::BufferDescriptor(TRIANGLE_INDICES, sizeof(TRIANGLE_INDICES), nullptr));
    LogToCSharp("Set index buffer");

    const filament::Material* material = engine->getDefaultMaterial();
    if (!material) {
        closeWindow(nullptr, "Default material not available");
    }
    LogToCSharp("Got default material");

    utils::Entity renderable = utils::EntityManager::get().create();
    if (renderable.isNull()) {
        closeWindow(nullptr, "Renderable not created");
    }
    LogToCSharp("Renderable created");

    filament::RenderableManager::Builder builder(1);
    builder
        .boundingBox({ { -1, -1, -1 }, { 1, 1, 1 } })
        .material(0, material->getDefaultInstance())
        .geometry(0, filament::RenderableManager::PrimitiveType::TRIANGLES, vb, ib, 0, 3)
        .culling(false)
        .receiveShadows(false)
        .castShadows(false);
    filament::RenderableManager::Builder::Result result = builder.build(*engine, renderable);
    if (result != filament::RenderableManager::Builder::Result::Success) {
        closeWindow(nullptr, "Renderable not built successfully");
    }
    LogToCSharp("Renderable built");

    scene->addEntity(renderable);
    LogToCSharp("Added renderable to scene");

    addLight(engine, scene);
    LogToCSharp("Added light to scene");

    view->setCamera(camera);
    LogToCSharp("Set camera to view");

    view->setScene(scene);
    LogToCSharp("Set scene to view");

    _engine = engine;
    _swapchain = swapChain;
    _renderer = renderer;
    _camera = camera;
    _view = view;
    _entity = renderable;
    _scene = scene;

    TransformManager& tcm = _engine->getTransformManager();
    LogToCSharp("Got transform manager");

    tcm.setTransform(tcm.getInstance(renderable),
        math::mat4f::rotation(M_PI_4, math::float3{ 0, 0, 1 }));
    LogToCSharp("Set transform");
}

void display() {
    if (_renderer->beginFrame(_swapchain)) {
        rotationAngle += 0.01f;

        TransformManager& tcm = _engine->getTransformManager();
        tcm.setTransform(tcm.getInstance(_entity),
            math::mat4f::rotation(rotationAngle, math::float3{ 0, 0, 1 }));

        _renderer->render(_view);

        _renderer->endFrame();
    }
}

void runWindow() {
    try {
        if (!glfwInit()) {
            closeWindow(nullptr, "GLFW not initialized");
        }
        LogToCSharp("GLFW initialized");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        LogToCSharp("Set GLFW client API to NO_API");

        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        LogToCSharp("Set GLFW window resizable to false");

        GLFWwindow* window = glfwCreateWindow(g_window_size_x, g_window_size_y, "Filament GLFW Example", nullptr, nullptr);
        if (window == nullptr) {
            closeWindow(nullptr, "Window not initialized");
        }
        LogToCSharp("Window created");

        glfwSetWindowSizeCallback(window, reshape_window);
        LogToCSharp("Set window size callback");

        glfwSetFramebufferSizeCallback(window, reshape_framebuffer);
        LogToCSharp("Set framebuffer size callback");

        glfwSetKeyCallback(window, key_press);
        LogToCSharp("Set key callback");

        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
        LogToCSharp("Set input mode to sticky keys");

        init(window);
        LogToCSharp("Initialized the renderer");

        auto lastFrameTime = std::chrono::high_resolution_clock::now();

        while (!glfwWindowShouldClose(window)) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = currentTime - lastFrameTime;

            if (elapsed.count() >= TARGET_FRAME_DURATION) {
                lastFrameTime = currentTime;
                LogToCSharp("Rendering frame");

                glfwPollEvents();
                display();
                glfwSwapBuffers(window);
            }
            else {
                std::this_thread::sleep_for(std::chrono::milliseconds(
                    static_cast<int>((TARGET_FRAME_DURATION - elapsed.count()) * 1000)));
            }
        }

        closeWindow(window, "Closing application");
    }
    catch (const std::exception& e) {
        closeWindow(nullptr, "Error in rendering thread: " + std::string(e.what()));
    }
    catch (...) {
        closeWindow(nullptr, "Error in rendering thread: Caught unknown exception");
    }
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

void closeWindow(GLFWwindow* window, std::string reason)
{
    LogToCSharp(reason);
    LogToCSharp("Initiating cleanup");

    if (_engine) {
        if (_renderer) {
            _engine->destroy(_renderer);
            LogToCSharp("Renderer destroyed");
        }
        if (_view) {
            _engine->destroy(_view);
            LogToCSharp("View destroyed");
        }
        if (_scene) {
            _engine->destroy(_scene);
            LogToCSharp("Scene destroyed");
        }
        if (_swapchain) {
            _engine->destroy(_swapchain);
            LogToCSharp("SwapChain destroyed");
        }
        filament::Engine::destroy(_engine);
        LogToCSharp("Engine destroyed");
    }

    if (window) {
        glfwDestroyWindow(window);
        LogToCSharp("GLFW window destroyed");
    }

    glfwTerminate();
    LogToCSharp("GLFW terminated");
}

