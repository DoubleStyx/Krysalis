// KrysalisNative.h
#pragma once
#define NOMINMAX
#include <math/vec2.h>
#include <math/compiler.h>
#include <GLFW/glfw3.h>
#include "uuid.h"
#include <unordered_map>
#include "KrysalisNative.h"
#include "KrysalisNative.h"
#include "Utils.h"
#include "SceneBuilder.h"
#include "TestUtils.h"

using namespace filament;
using namespace math;
using namespace utils;
using namespace filamesh;

extern double TARGET_FPS;
extern double TARGET_FRAME_DURATION;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_4
#define M_PI_4 0.78539816339744830962
#endif

extern filament::Engine* _engine;
extern filament::SwapChain* _swapchain; 
extern filament::Renderer* _renderer;
extern filament::Camera* _camera;
extern filament::View* _view;
extern filament::Scene* _scene;

extern std::unordered_map<uuids::uuid, utils::Entity> _entities;

extern int g_window_size_x;
extern int g_window_size_y;
extern int g_frame_size_x;
extern int g_frame_size_y;

void closeWindow(GLFWwindow* window, std::string reason);
void runWindow();
void display();
void init(GLFWwindow* window);
void key_press(GLFWwindow* window, int key, int scancode, int action, int mods);
void reshape_framebuffer(GLFWwindow* window, int w, int h);
void reshape_window(GLFWwindow* window, int w, int h);
void* getNativeWindow(GLFWwindow* window);
void setCallbacks(GLFWwindow* window);