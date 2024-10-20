// SceneBuilder.h
#pragma once
#include <rapidjson/document.h>
#include <filament/Engine.h>
#include <filament/Scene.h>
#include <filament/LightManager.h>
#include "KrysalisNative.h"
#include "Utils.h"
#include "SceneBuilder.h"

rapidjson::Document loadSceneFromFile(const std::string& relativePath);
void createScene(filament::Engine* engine, filament::Scene* scene, const rapidjson::Document& sceneData);
filament::LightManager::Builder createLightComponent(const rapidjson::Value& obj);
void createMeshComponent(filament::Engine* engine, filament::Scene* scene, const rapidjson::Value& obj, utils::Entity entity);
void applyTransform(filament::Engine* engine, const rapidjson::Value& obj, utils::Entity entity);