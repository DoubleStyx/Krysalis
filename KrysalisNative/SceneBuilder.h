// SceneBuilder.h
#pragma once
#include <nlohmann/json.hpp>
#include "KrysalisNative.h"
#include "Utils.h"
#include "SceneBuilder.h"

nlohmann::json loadSceneFromFile(const std::string& filename);
void createScene(filament::Engine* engine, filament::Scene* scene, const nlohmann::json& sceneData);
filament::LightManager::Builder createLightComponent(const nlohmann::json& obj);
void createMeshComponent(filament::Engine* engine, filament::Scene* scene, const nlohmann::json& obj, utils::Entity entity);
void applyTransform(filament::Engine* engine, const nlohmann::json& obj, utils::Entity entity);
void createMaterialComponent(filament::Engine* engine, filament::Scene* scene, const nlohmann::json& obj, utils::Entity entity);
void CreateTextureComponent(); // lots of void returns; maybe have a better way to report errors?
void updateScene();
void loadScene();