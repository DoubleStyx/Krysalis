// SceneBuilder.h
#pragma once
#define NOMINMAX
#include "KrysalisNative.h"
#include "Utils.h"
#include "SceneBuilder.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filament/Engine.h>
#include <filament/Scene.h>
#include <filament/RenderableManager.h>
#include <filament/TransformManager.h>
#include <filament/LightManager.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/Texture.h>
#include <filameshio/MeshReader.h>
#include <filament/TextureSampler.h>
#include <math/mat4.h>
#include <math/vec4.h>
#include <math/vec3.h>
#include <math/quat.h>
#include <utils/EntityManager.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream> // deduplicate
#include <stb_image.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

rapidjson::Document loadSceneFromFile(const std::string& relativePath);
void createScene(filament::Engine* engine, filament::Scene* scene, const rapidjson::Document& sceneData);
filament::LightManager::Builder createLightComponent(const rapidjson::Value& obj);
void createMeshComponent(filament::Engine* engine, filament::Scene* scene, const rapidjson::Value& obj, utils::Entity entity);
void applyTransform(filament::Engine* engine, const rapidjson::Value& obj, utils::Entity entity);
filament::Texture* loadTexture(filament::Engine* engine, const std::wstring& relativePath);
std::vector<uint8_t> loadFile(const std::wstring& relativePath);
filament::Material* loadMaterial(filament::Engine* engine, const std::string& materialURI);
filamesh::MeshReader::Mesh loadMeshFromFile(filament::Engine* engine, const std::string& meshURI,
    filamesh::MeshReader::MaterialRegistry& materialRegistry);
void updateScene();
void loadScene(filament::Engine* engine, filament::Scene* scene);
void VerifyComponents(filament::Engine* engine); // TEMPORARY