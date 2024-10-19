// Utils.h
#pragma once
#include <filament/Engine.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <utils/Path.h>
#include <fstream>
#include <vector>

std::vector<uint8_t> loadFile(const std::string& filePath);

filament::Texture* loadTexture(filament::Engine* engine, const std::string& filePath);