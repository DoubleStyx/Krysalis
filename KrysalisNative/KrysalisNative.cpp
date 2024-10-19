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
#include "KrysalisNative.h"
#include "Utils.h"

using namespace filament;
using namespace math;

double TARGET_FPS = 60.0;
double TARGET_FRAME_DURATION = 1.0 / TARGET_FPS;

struct Vertex {
    math::float2 position;
    uint32_t color;
};

filament::Engine* _engine = nullptr;
filament::SwapChain* _swapchain = nullptr;
filament::Renderer* _renderer = nullptr;
filament::Camera* _camera = nullptr;
filament::View* _view = nullptr;
filament::Scene* _scene = nullptr;
utils::Entity _entity;

int g_window_size_x = 512;
int g_window_size_y = 512;
int g_frame_size_x = g_window_size_x;
int g_frame_size_y = g_window_size_y;

float rotationAngle = 0.0f;

void* getNativeWindow(GLFWwindow* window) {
    GlobalLog("Getting native window");

    HWND hwnd = glfwGetWin32Window(window);
    if (hwnd == nullptr) {
        closeWindow(nullptr, "Native window not found");
    }
    GlobalLog("Got native window");

    return hwnd;
}

void reshape_window(GLFWwindow* window, int w, int h) {
    GlobalLog("Reshaping window");

    (void)window; (void)w; (void)h;
    GlobalLog("Reshaped window");
}

void reshape_framebuffer(GLFWwindow* window, int w, int h) {
    (void)window; (void)w; (void)h;
	GlobalLog("Reshaped framebuffer");
}

void key_press(GLFWwindow* window, int key, int scancode, int action, int mods) {

    (void)window; (void)mods; (void)scancode;

    if (action != GLFW_PRESS) {

        return;
    }

    if (key == GLFW_KEY_Q) {
        GlobalLog("Key Q pressed");

        closeWindow(window, "Closing window");
    }
}

void addLight(filament::Engine* engine, filament::Scene* scene) {
    utils::Entity lightEntity = utils::EntityManager::get().create();
    if (lightEntity.isNull()) {
        closeWindow(nullptr, "Light entity not created");
    }
    GlobalLog("Light entity created");

    LightManager::Builder(LightManager::Type::POINT)
        .color({ 0.95f, 0.9f, 0.85f })
        .intensity(100000.0f)
        .falloff(100.0f)
        .build(*engine, lightEntity);
    GlobalLog("Built light entity");

    TransformManager& tcm = engine->getTransformManager();
    tcm.create(lightEntity);
    GlobalLog("Created Transform component for light entity");

    TransformManager::Instance lightInstance = tcm.getInstance(lightEntity);
    if (lightInstance.isValid()) {
        tcm.setTransform(lightInstance, math::mat4f::translation(math::float3{ 0.0f, 0.0f, 10.0f }));
        GlobalLog("Set light position to (0, 0, 10)");
    }
    else {
        GlobalLog("Failed to get TransformManager instance for light entity");
    }

    scene->addEntity(lightEntity);
    GlobalLog("Added light entity to scene");
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

    {
        math::double3 eye(0.0, 0.0, 3.0);
        math::double3 at(0.0, 0.0, 0.0);
        math::double3 up(0.0, 1.0, 0.0);

        math::mat4 viewMatrix = math::mat4::lookAt(eye, at, up);
        camera->setModelMatrix(math::mat4f(viewMatrix));
        GlobalLog("Set camera model matrix using lookAt");
    }

    camera->setProjection(
        45.0f,
        static_cast<float>(g_frame_size_x) / g_frame_size_y,
        0.1f,
        100.0f,
        filament::Camera::Fov::VERTICAL
    );
    GlobalLog("Set camera projection (Perspective)");

    camera->lookAt(
        math::float3{ 0.0f, 0.0f, 5.0f },
        math::float3{ 0.0f, 0.0f, 0.0f },
        math::float3{ 0.0f, 1.0f, 0.0f });
    GlobalLog("Camera positioned");

    renderer->setClearOptions({
        .clearColor = {0.0f, 0.0f, 0.0f, 1.0f},
        .clear = true
        });
    GlobalLog("Set clear options");

    view->setPostProcessingEnabled(true);
    GlobalLog("Set post processing flag");

    std::vector<uint8_t> materialData = loadFile(L"materials\\texturedLit.filamat");
	if (materialData.empty())
		closeWindow(nullptr, "Material data not loaded");
    GlobalLog("Loaded material data of size " + std::to_string(materialData.size()));
    
    filament::Material* material = filament::Material::Builder()
        .package(materialData.data(), materialData.size())
        .build(*engine);
    if (material == nullptr)
        closeWindow(nullptr, "Material not created");
    GlobalLog("Material created");

    filament::MaterialInstance* materialInstance = material->createInstance();
    if (materialInstance == nullptr)
        closeWindow(nullptr, "Material instance not created");
    GlobalLog("Material instance created");

	Texture* albedoTexture = loadTexture(engine, L"textures\\color.png");
	if (albedoTexture == nullptr)
		closeWindow(nullptr, "Albedo texture not loaded");
	GlobalLog("Loaded albedo texture");

	Texture* normalTexture = loadTexture(engine, L"textures\\normal.png");
	if (normalTexture == nullptr)
		closeWindow(nullptr, "Normal texture not loaded");
	GlobalLog("Loaded normal texture");

	Texture* roughnessTexture = loadTexture(engine, L"textures\\roughness.png");
	if (roughnessTexture == nullptr)
		closeWindow(nullptr, "Roughness texture not loaded");
	GlobalLog("Loaded roughness texture");

	Texture* metallicTexture = loadTexture(engine, L"textures\\metallic.png");
	if (metallicTexture == nullptr)
		closeWindow(nullptr, "Metallic texture not loaded");
	GlobalLog("Loaded metallic texture");

	Texture* aoTexture = loadTexture(engine, L"textures\\ao.png");
	if (aoTexture == nullptr)
		closeWindow(nullptr, "AO texture not loaded");
	GlobalLog("Loaded AO texture");

    TextureSampler sampler(TextureSampler::MinFilter::LINEAR_MIPMAP_LINEAR,
        TextureSampler::MagFilter::LINEAR);

    materialInstance->setParameter("albedo", albedoTexture, sampler);
    GlobalLog("Set albedo texture");

	materialInstance->setParameter("normal", normalTexture, sampler);
	GlobalLog("Set normal texture");

	materialInstance->setParameter("roughness", roughnessTexture, sampler);
	GlobalLog("Set roughness texture");

	materialInstance->setParameter("metallic", metallicTexture, sampler);
	GlobalLog("Set metallic texture");

	materialInstance->setParameter("ao", aoTexture, sampler);
	GlobalLog("Set AO texture");

    filamesh::MeshReader::MaterialRegistry registry;
	registry.registerMaterialInstance("TexturedLit", materialInstance);
	GlobalLog("Registered material instance");

    filamesh::MeshReader::Mesh mesh = filamesh::MeshReader::loadMeshFromFile(engine, utils::Path(wstringToString(getFullPath(L"meshes\\monkey.filamesh"))), registry);
	if (mesh.renderable.isNull())
		closeWindow(nullptr, "Mesh not loaded");
	GlobalLog("Loaded mesh");

    scene->addEntity(mesh.renderable);
	GlobalLog("Added mesh to scene");

    addLight(engine, scene);
    GlobalLog("Added light to scene");

    view->setCamera(camera);
    GlobalLog("Set camera to view");

    view->setScene(scene);
    GlobalLog("Set scene to view");

    _engine = engine;
    _swapchain = swapChain;
    _renderer = renderer;
    _camera = camera;
    _view = view;
    _entity = mesh.renderable;
    _scene = scene;

    TransformManager& tcm = _engine->getTransformManager();
    GlobalLog("Got transform manager");

    tcm.setTransform(tcm.getInstance(_entity),
        math::mat4f::rotation(M_PI_4, math::float3{ 0, 1, 1 }));
    GlobalLog("Set transform");
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

void closeWindow(GLFWwindow* window, std::string reason)
{
    GlobalLog(reason);
    GlobalLog("Initiating cleanup");

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

    GlobalLog("Closing application");
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

        glfwSetWindowSizeCallback(window, reshape_window);
        GlobalLog("Set window size callback");

        glfwSetFramebufferSizeCallback(window, reshape_framebuffer);
        GlobalLog("Set framebuffer size callback");

        glfwSetKeyCallback(window, key_press);
        GlobalLog("Set key callback");

        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
        GlobalLog("Set input mode to sticky keys");

        init(window);
        GlobalLog("Initialized the renderer");

        auto lastFrameTime = std::chrono::high_resolution_clock::now();
		GlobalLog("Saved last frame time");

		GlobalLog("Renderer and window initialized successfully! Beginning main rendering and interaction loop.");
        while (!glfwWindowShouldClose(window)) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = currentTime - lastFrameTime;

            if (elapsed.count() >= TARGET_FRAME_DURATION) {
                lastFrameTime = currentTime;

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