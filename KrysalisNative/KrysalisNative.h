// KrysalisNative.h
#pragma once
#include "KrysalisNative.h"
#include "Utils.h"
#include <GLFW/glfw3.h>

extern std::mutex logMutex;

void closeWindow(GLFWwindow* window, std::string reason);
void runWindow();
void display();
void init(GLFWwindow* window);
void addLight(filament::Engine* engine, filament::Scene* scene);
void key_press(GLFWwindow* window, int key, int scancode, int action, int mods);
void reshape_framebuffer(GLFWwindow* window, int w, int h);
void reshape_window(GLFWwindow* window, int w, int h);
void* getNativeWindow(GLFWwindow* window);