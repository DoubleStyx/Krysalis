// SceneBuilder.cpp
// scene loading and updating

#include "KrysalisNative.h"
#include "Utils.h"
#include "SceneBuilder.h"
#include <nlohmann/json.hpp>
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
#include <utils/EntityManager.h>

using namespace filament;

// auto sort and format cpp files?

void updateScene() {
    // Update the scene from changesets
}

void loadScene() {
    loadSceneFromFile("scenes\\scene.json"); // load from file since we don't have dynamic loading yet
}

nlohmann::json loadSceneFromFile(const std::string& relativePath) {
    // Open the file using ifstream
    std::ifstream file(wstringToString(getFullPath(stringToWstring(relativePath))));

    // Check if the file is open and valid
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + relativePath);
    }

    // Read the JSON data
    nlohmann::json sceneData;
    file >> sceneData;

    // Return the loaded JSON data
    return sceneData;
}

void createScene(filament::Engine* engine, filament::Scene* scene, const nlohmann::json& sceneData) {
    utils::EntityManager& em = utils::EntityManager::get();

    for (const auto& obj : sceneData["objects"]) {
        // Create a new entity
        utils::Entity entity = em.create();
        std::string type = obj["type"];

        // Handle light components
        if (type == "light") {
            filament::LightManager::Builder lightBuilder = createLightComponent(obj);
            lightBuilder.build(*engine, entity);
        }
        // Handle mesh components
        else if (type == "mesh") {
            createMeshComponent(engine, scene, obj, entity);
        }

        // Apply the transform (position, rotation, scale)
        applyTransform(engine, obj, entity);

        // Add the entity to the scene
        scene->addEntity(entity);
    }
}

filament::LightManager::Builder createLightComponent(const nlohmann::json& obj) {
    /*
    utils::Entity lightEntity = utils::EntityManager::get().create();
    if (lightEntity.isNull()) {
        closeWindow(nullptr, "Light entity not created");
    }
    GlobalLog("Light entity created");

    LightManager::Builder(LightManager::Type::DIRECTIONAL)
        .color({ 1.0f, 1.0f, 1.0f })
        .intensity(1000000.0f)
        .direction({ 0.0f, -0.5f, -1.0f })
        .castShadows(true)
        .build(*engine, lightEntity);
    GlobalLog("Built light entity");

    scene->addEntity(lightEntity);
    GlobalLog("Added light entity to scene");
    */
    filament::LightManager::Builder builder(
        obj["component"]["lightType"] == "directional"
        ? filament::LightManager::Type::DIRECTIONAL
        : filament::LightManager::Type::POINT);

    auto color = obj["component"]["color"];
    builder.color({ color[0], color[1], color[2] })
        .intensity(obj["component"]["intensity"]);

    if (obj["component"]["lightType"] == "point") {
        builder.falloff(obj["component"]["falloff"]);
    }

    if (obj["component"].contains("castShadows") && obj["component"]["castShadows"]) {
        builder.castShadows(true);
    }

    return builder;
}

void createMeshComponent(filament::Engine* engine, filament::Scene* scene, const nlohmann::json& obj, utils::Entity entity) {
    /*
    filamesh::MeshReader::Mesh mesh = filamesh::MeshReader::loadMeshFromFile(engine, utils::Path(wstringToString(getFullPath(L"meshes\\suzanne.filamesh"))), registry);
    if (mesh.renderable.isNull())
        closeWindow(nullptr, "Mesh not loaded");
    GlobalLog("Loaded mesh");

    scene->addEntity(mesh.renderable);
    GlobalLog("Added mesh to scene");
    */
    std::string meshURI = obj["component"]["meshURI"];

    // Load the mesh (use a function to handle this)
    filament::VertexBuffer* vb;
    filament::IndexBuffer* ib;
    loadMeshFromFile(engine, meshURI, &vb, &ib);

    // Create material instances from the JSON
    const json& materials = obj["component"]["materials"];
    for (const auto& materialJson : materials) {
        std::string materialURI = materialJson["materialURI"];

        filament::Material* material = loadMaterial(engine, materialURI);
        filament::MaterialInstance* materialInstance = material->createInstance();

        // Apply parameters
        for (const auto& param : materialJson["parameters"]) {
            std::string paramName = param["name"];
            if (param["type"] == "sampler2d") {
                filament::Texture* texture = loadTexture(engine, param["value"]);
                materialInstance->setParameter(paramName.c_str(), texture, filament::TextureSampler());
            }
            else if (param["type"] == "float") {
                materialInstance->setParameter(paramName.c_str(), param["value"].get<float>());
            }
            else if (param["type"] == "float3") {
                materialInstance->setParameter(paramName.c_str(), math::float3(param["value"][0], param["value"][1], param["value"][2]));
            }
            else if (param["type"] == "float4") {
                materialInstance->setParameter(paramName.c_str(), math::float4(param["value"][0], param["value"][1], param["value"][2], param["value"][3]));
            }
        }

        // Build the renderable component
        filament::RenderableManager::Builder(1)
            .boundingBox({ {0, 0, 0}, {1, 1, 1} })
            .geometry(0, filament::RenderableManager::PrimitiveType::TRIANGLES, vb, ib)
            .material(0, materialInstance)
            .build(*engine, entity);
    }
}

void applyTransform(filament::Engine* engine, const nlohmann::json& obj, utils::Entity entity) {
    filament::TransformManager& tcm = engine->getTransformManager();

    auto position = obj["transform"]["position"];
    auto rotation = obj["transform"]["rotation"];
    auto scale = obj["transform"]["scale"];

    math::mat4f translation = math::mat4f::translation({ position[0], position[1], position[2] });
    math::mat4f rotationMat = math::mat4f::rotationEulerXYZ(rotation[0], rotation[1], rotation[2]);
    math::mat4f scaleMat = math::mat4f::scaling({ scale[0], scale[1], scale[2] });

    // Combine translation, rotation, and scale into one transform matrix
    math::mat4f transform = translation * rotationMat * scaleMat;

    tcm.setTransform(tcm.getInstance(entity), transform);
}


void createMaterialComponent(filament::Engine* engine, filament::Scene* scene, const nlohmann::json& obj, utils::Entity entity) {
    /*
    std::vector<uint8_t> materialData = loadFile(L"materials\\texturedLitEmissive.filamat");
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

    filamesh::MeshReader::MaterialRegistry registry;
    registry.registerMaterialInstance("Unlit", materialInstance);
    GlobalLog("Registered material instance");
    */
}

void CreateTextureComponent() {
    /*
    
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
    */


}


void SetParametersOnMaterial() {
    /*
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

    materialInstance->setParameter("clearCoat", 0.0f);
	GlobalLog("Set clear coat");

	materialInstance->setParameter("emissive", math::float4{ 0.0f, 0.0f, 0.0f, 1.0f });
	GlobalLog("Set emissive");
    */
}