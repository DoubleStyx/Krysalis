// KrysalisNative.cpp
// main rendering loop, pipeline initialization, window creation/management
#define NOMINMAX
#define GLFW_EXPOSE_NATIVE_WIN32
// clean up headers at some point, perhaps with a tool
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
#include <filament/Skybox.h>
#include <filament/LightManager.h>
#include <filament/Material.h>
#include <filament/RenderableManager.h>
#include <filament/Texture.h>
#include <filameshio/MeshReader.h>
#include <filament/TextureSampler.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/Viewport.h>
#include <filament/IndirectLight.h>
#include <filameshio/MeshReader.h>
#include <fstream>
#include <vector>
#include <filamat/Enums.h>
#include <filamat/IncludeCallback.h>
#include <filamat/MaterialBuilder.h>
#include <filamat/Package.h>
#include <utils/EntityManager.h>
#include <utils/Path.h>
#include <Windows.h>
#include <chrono>
#include <thread>
#include "uuid.h"
#include "KrysalisNative.h"
#include "Utils.h"
#include "SceneBuilder.h"

// other namespaces worth including? Maybe simplify namespace usage? Don't want conflicts or ambiguity though.
using namespace filament;
using namespace math;
using namespace utils;
using namespace filamesh;

double TARGET_FPS = 60.0; // get screen refresh rate eventually
double TARGET_FRAME_DURATION = 1.0 / TARGET_FPS;

filament::Engine* _engine = nullptr;
filament::SwapChain* _swapchain = nullptr;
filament::Renderer* _renderer = nullptr;
filament::Camera* _camera = nullptr;
filament::View* _view = nullptr;
filament::Scene* _scene = nullptr;

std::unordered_map<uuids::uuid, utils::Entity> _entities;

// Get monitor resolution eventually
int g_window_size_x = 512;
int g_window_size_y = 512;
int g_frame_size_x = g_window_size_x;
int g_frame_size_y = g_window_size_y;

// can this be moved elsewhere?
void* getNativeWindow(GLFWwindow* window) {
    HWND hwnd = glfwGetWin32Window(window); // We're focused on Windows support for now
    if (hwnd == nullptr) {
        closeWindow(nullptr, "Native window not found");
    }
    GlobalLog("Got native window");

    return hwnd;
}

// Do something here
void reshape_window(GLFWwindow* window, int w, int h) {
    (void)window; (void)w; (void)h;
    GlobalLog("Reshaped window");
}

// Do something here
void reshape_framebuffer(GLFWwindow* window, int w, int h) {
    (void)window; (void)w; (void)h;
	GlobalLog("Reshaped framebuffer");
}

// Get all key and mouse events and send to resonite
void key_press(GLFWwindow* window, int key, int scancode, int action, int mods) {

    (void)window; (void)mods; (void)scancode;
	GlobalLog("Key pressed");

    /*
    if (action != GLFW_PRESS) {
        return;
    }
    */ // needed for now?

    if (key == GLFW_KEY_Q) {
        GlobalLog("Key Q pressed");

        closeWindow(window, "Window close initiated from keypress");
    }
}

void init(GLFWwindow* window) {
    filament::Engine* engine = filament::Engine::create(Engine::Backend::OPENGL);
    if (!engine) {
        closeWindow(nullptr, "Engine not created");
    }
    GlobalLog("Engine created");

    filament::SwapChain* swapChain = engine->createSwapChain(getNativeWindow(window));
    if (!swapChain) {
        closeWindow(nullptr, "Swap chain not created");
    }
    GlobalLog("Swap chain created");

    filament::Renderer* renderer = engine->createRenderer();
    if (!renderer) {
        closeWindow(nullptr, "Renderer not created");
    }
    GlobalLog("Renderer created");

    utils::Entity cameraEntity = utils::EntityManager::get().create();
    if (cameraEntity.isNull()) {
        closeWindow(nullptr, "Camera entity not created");
    }
    GlobalLog("Camera entity created");

    Camera* camera = engine->createCamera(cameraEntity);
    if (!camera) {
        closeWindow(nullptr, "Camera not created");
    }
    GlobalLog("Camera created");

    filament::View* view = engine->createView();
    if (!view) {
        closeWindow(nullptr, "View not created");
    }
    GlobalLog("View created");

    filament::Scene* scene = engine->createScene();
    if (!scene) {
        closeWindow(nullptr, "Scene not created");
    }
    GlobalLog("Scene created");

    view->setViewport({ 0, 0, static_cast<uint32_t>(g_frame_size_x), static_cast<uint32_t>(g_frame_size_y) });
    GlobalLog("Set viewport");

    // Eventually get camera from Resonite's active camera
    // We let resonite handle the camera transform
    {
        math::double3 eye(0.0, 0.0, 3.0);
        math::double3 at(0.0, 0.0, 0.0);
        math::double3 up(0.0, 1.0, 0.0);

        math::mat4 viewMatrix = math::mat4::lookAt(eye, at, up);
        camera->setModelMatrix(math::mat4f(viewMatrix));
        GlobalLog("Set camera model matrix using lookAt");
    }
    
    camera->setProjection(
        60.0f,
        static_cast<float>(g_frame_size_x) / g_frame_size_y,
        0.1f,
        100.0f,
        filament::Camera::Fov::VERTICAL
    );
    GlobalLog("Set camera projection (Perspective)");

    camera->lookAt(
        math::float3{ 0.0f, 0.0f, 5.0f },
        math::float3{ 0.0f, 0.0f, 0.0f },
        math::float3{ 0.0f, 1.0f, 0.0f }
    );
    GlobalLog("Camera positioned");

    renderer->setClearOptions({
        .clearColor = {0.0f, 0.0f, 0.0f, 1.0f},
        .clear = true
        });
    GlobalLog("Set clear options");

    view->setPostProcessingEnabled(true);
    GlobalLog("Set post processing flag");

    view->setCamera(camera);
    GlobalLog("Set camera to view");

    view->setScene(scene);
    GlobalLog("Set scene to view");

    loadScene(engine, scene);
    GlobalLog("Loaded scene");

    _engine = engine;
    _swapchain = swapChain;
    _renderer = renderer;
    _camera = camera;
    _view = view;
    _scene = scene;
	GlobalLog("Assigned singletons");
}

void display() {
    if (_renderer->beginFrame(_swapchain)) {

        _renderer->render(_view);

        _renderer->endFrame();
    }
}

void closeWindow(GLFWwindow* window, std::string reason)
{
    GlobalLog(reason);

    if (_engine) {
        if (_renderer) {
            _engine->destroy(_renderer);
            GlobalLog("Renderer destroyed");
        }
        if (_view) {
            _engine->destroy(_view);
            GlobalLog("View destroyed");
        }
        if (_scene) {
            _engine->destroy(_scene);
            GlobalLog("Scene destroyed");
        }
        if (_swapchain) {
            _engine->destroy(_swapchain);
            GlobalLog("SwapChain destroyed");
        }
        filament::Engine::destroy(_engine);
        GlobalLog("Engine destroyed");
    }

    if (window) {
        glfwDestroyWindow(window);
        GlobalLog("GLFW window destroyed");
    }

    glfwTerminate();
    GlobalLog("GLFW terminated");

	closeLogFile();
    exit(0);
}

void runWindow() {
    try {
        if (!glfwInit()) {
            closeWindow(nullptr, "GLFW not initialized");
        }
        GlobalLog("GLFW initialized");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        GlobalLog("Set GLFW client API to NO_API");

        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        GlobalLog("Set GLFW window resizable to false");

        GLFWwindow* window = glfwCreateWindow(g_window_size_x, g_window_size_y, "Filament GLFW Example", nullptr, nullptr);
        if (window == nullptr) {
            closeWindow(nullptr, "Window not initialized");
        }
        GlobalLog("Window created");

        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
        GlobalLog("Set input mode to sticky keys");

        setCallbacks(window);
		GlobalLog("Set callbacks");

        init(window);
        GlobalLog("Initialized the renderer");

        auto lastFrameTime = std::chrono::high_resolution_clock::now();
		GlobalLog("Saved last frame time");

		std::chrono::steady_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currentTime - lastFrameTime;
		GlobalLog("Calculated elapsed time");

        while (!glfwWindowShouldClose(window)) {
            currentTime = std::chrono::high_resolution_clock::now();
            elapsed = currentTime - lastFrameTime;

            if (elapsed.count() >= TARGET_FRAME_DURATION) {
                lastFrameTime = currentTime;

                updateScene();
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

void setCallbacks(GLFWwindow* window) {
    glfwSetCharCallback(window, nullptr);
	GlobalLog("Set char callback");

    glfwSetCharModsCallback(window, nullptr);
	GlobalLog("Set char mods callback");

    glfwSetCursorEnterCallback(window, nullptr);
	GlobalLog("Set cursor enter callback");

    glfwSetCursorPosCallback(window, nullptr);
	GlobalLog("Set cursor pos callback");

    glfwSetDropCallback(window, nullptr);
	GlobalLog("Set drop callback");

    glfwSetErrorCallback(nullptr);
	GlobalLog("Set error callback");

    glfwSetFramebufferSizeCallback(window, reshape_framebuffer);
	GlobalLog("Set framebuffer size callback");

    glfwSetKeyCallback(window, key_press);
	GlobalLog("Set key callback");

    glfwSetMonitorCallback(nullptr);
	GlobalLog("Set monitor callback");

    glfwSetMouseButtonCallback(window, nullptr);
	GlobalLog("Set mouse button callback");

    glfwSetScrollCallback(window, nullptr);
	GlobalLog("Set scroll callback");

    glfwSetWindowCloseCallback(window, nullptr);
	GlobalLog("Set window close callback");

    glfwSetWindowContentScaleCallback(window, nullptr);
	GlobalLog("Set window content scale callback");

    glfwSetWindowFocusCallback(window, nullptr);
	GlobalLog("Set window focus callback");

    glfwSetWindowIconifyCallback(window, nullptr);
    GlobalLog("Set window iconify callback");

    glfwSetWindowMaximizeCallback(window, nullptr);
	GlobalLog("Set window maximize callback");

    glfwSetWindowPosCallback(window, nullptr);
	GlobalLog("Set window pos callback");

    glfwSetWindowRefreshCallback(window, nullptr);
	GlobalLog("Set window refresh callback");

    glfwSetWindowSizeCallback(window, reshape_window);
	GlobalLog("Set window size callback");
}