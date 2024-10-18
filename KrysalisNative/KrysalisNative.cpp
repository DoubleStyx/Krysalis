// KrysalisNative.cpp
#include "KrysalisNative.h"
#include <iostream>
#include <filament/Engine.h>

int Add(int a, int b) {
    return a + b;
}

bool StartEngine() {
    filament::Engine* engine = filament::Engine::create();
    return true;
}