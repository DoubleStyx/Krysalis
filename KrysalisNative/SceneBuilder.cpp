// SceneBuilder.cpp
// scene loading and updating
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
#include <utils/Entity.h>
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

// namespaces?
using namespace filament;
using namespace math;
using namespace utils;
using namespace filamesh;

// auto sort and format cpp files?
// monad pattern? To have better logging?
// more informative logging?

void updateScene() {
    // Update the scene from event-driven changesets
}

void loadScene(filament::Engine* engine, filament::Scene* scene) {
    rapidjson::Document document = loadSceneFromFile("..\\..\\assets\\scenes\\testScene.json"); // load from file since we don't have dynamic loading yet
    GlobalLog("Loaded scene from file");

    createScene(engine, scene, document);
	GlobalLog("Created scene");
}

rapidjson::Document loadSceneFromFile(const std::string& relativePath) {
    std::ifstream file(wstringToString(getFullPath(stringToWstring(relativePath))));
	GlobalLog("Opened file: " + wstringToString(getFullPath(stringToWstring(relativePath))));

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + relativePath);
    }

    rapidjson::IStreamWrapper isw(file);
	GlobalLog("Created IStreamWrapper");

    rapidjson::Document sceneData;
    sceneData.ParseStream(isw);
	GlobalLog("Parsed stream");

    if (sceneData.HasParseError()) {
        throw std::runtime_error("Error parsing JSON file: " + relativePath);
    }

    return sceneData;
}

void createScene(filament::Engine* engine, filament::Scene* scene, const rapidjson::Document& sceneData) {
    utils::EntityManager& em = utils::EntityManager::get();
	GlobalLog("Created EntityManager");

    const rapidjson::Value& objects = sceneData["objects"];
	GlobalLog("Obtained scene objects");
    for (rapidjson::SizeType i = 0; i < objects.Size(); i++) {
        const rapidjson::Value& obj = objects[i];
		GlobalLog("Processing object " + std::to_string(i));

        utils::Entity entity;

        std::string type = obj["type"].GetString();
		GlobalLog("Obtained object type");

        if (type == "light") {
            entity = createLightComponent(engine, obj);
			GlobalLog("Created light component");
        }
        else if (type == "mesh") {
            entity = createMeshComponent(engine, obj);
			GlobalLog("Created mesh component");
        }

        applyTransform(engine, obj, entity);
		GlobalLog("Applied transform");

        scene->addEntity(entity);
		GlobalLog("Added entity to scene");

        auto uuidOpt = uuids::uuid::from_string(obj["id"].GetString());
		GlobalLog("Obtained UUID string");

        if (uuidOpt) { // add more checks? what else can fail? add checks upfront or any where issues occur?
            _entities[uuidOpt.value()] = entity;
			GlobalLog("Added entity to entities map");
        }
        else {
            GlobalLog("Invalid UUID string");
        }
    }
	GlobalLog("Finished processing scene objects");

	VerifyComponents(engine);  // TEMPORARY
	GlobalLog("Verified components");
}

void VerifyComponents(filament::Engine* engine) { // TEMPORARY
    TransformManager& transformManager = engine->getTransformManager();
	GlobalLog("Obtained TransformManager");

    RenderableManager& renderableManager = engine->getRenderableManager();
	GlobalLog("Obtained RenderableManager");

    LightManager& lightManager = engine->getLightManager(); // can probably remove excessive logging for actions that don't fail
	GlobalLog("Obtained LightManager");

    for (const auto& [uuid, entity] : _entities) {
        GlobalLog("Entity UUID: " + to_string(uuid));

        if (transformManager.hasComponent(entity)) {
            GlobalLog("  - Has Transform Component");
        }
        else {
            GlobalLog("  - Missing Transform Component");
        }

        if (renderableManager.hasComponent(entity)) {
            GlobalLog("  - Has Renderable Component");
        }
        else {
            GlobalLog("  - Missing Renderable Component");
        }

        if (lightManager.hasComponent(entity)) {
            GlobalLog("  - Has Light Component");
        }
        else {
            GlobalLog("  - Missing Light Component");
        }
    }
}

utils::Entity createLightComponent(Engine* engine, const rapidjson::Value& obj) {
	utils::Entity entity = utils::EntityManager::get().create();

    filament::LightManager::Builder builder(
        std::string(obj["component"]["lightType"].GetString()) == "directional"
        ? filament::LightManager::Type::DIRECTIONAL
        : filament::LightManager::Type::POINT);
	GlobalLog("Created light manager builder");

    const rapidjson::Value& color = obj["component"]["color"];
	GlobalLog("Obtained color");

    builder.color({ color[0].GetFloat(), color[1].GetFloat(), color[2].GetFloat() })
        .intensity(obj["component"]["intensity"].GetFloat());
	GlobalLog("Set color and intensity");

    if (std::string(obj["component"]["lightType"].GetString()) == "point") {
        builder.falloff(obj["component"]["falloff"].GetFloat());
		GlobalLog("Set falloff");
    }

    if (obj["component"].HasMember("castShadows") && obj["component"]["castShadows"].GetBool()) {
        builder.castShadows(true);
		GlobalLog("Set cast shadows");
    }

    builder.build(*engine, entity);
	GlobalLog("Built light component");

	return entity;
}

utils::Entity createMeshComponent(filament::Engine* engine, const rapidjson::Value& obj) {
    std::string meshURI = obj["component"]["meshURI"].GetString();
    GlobalLog("Mesh URI: " + meshURI);

    filamesh::MeshReader::MaterialRegistry materialRegistry;

    const rapidjson::Value& materials = obj["component"]["materials"];
    GlobalLog("Obtained materials");

    for (rapidjson::SizeType i = 0; i < materials.Size(); i++) {
        const rapidjson::Value& materialJson = materials[i];
        GlobalLog("Processing material " + std::to_string(i));

        std::string materialURI = materialJson["materialURI"].GetString();
        GlobalLog("Material URI: " + materialURI);

        filament::Material* material = loadMaterial(engine, materialURI);
        GlobalLog("Loaded material");

        filament::MaterialInstance* materialInstance = material->createInstance();
        GlobalLog("Created material instance");

        const rapidjson::Value& parameters = materialJson["parameters"];
        GlobalLog("Obtained parameters");

        for (rapidjson::SizeType j = 0; j < parameters.Size(); j++) {
            const rapidjson::Value& param = parameters[j];
            GlobalLog("Processing parameter " + std::to_string(j));

            std::string paramName = param["name"].GetString();
            GlobalLog("Parameter name: " + paramName);

            if (std::string(param["type"].GetString()) == "sampler2d") {
                filament::Texture* texture = loadTexture(engine, stringToWstring(param["value"].GetString()));
                GlobalLog("Loaded texture");
                materialInstance->setParameter(paramName.c_str(), texture, filament::TextureSampler());
            }
            else if (std::string(param["type"].GetString()) == "float") {
                materialInstance->setParameter(paramName.c_str(), param["value"].GetFloat());
            }
            else if (std::string(param["type"].GetString()) == "float3") {
                materialInstance->setParameter(paramName.c_str(), math::float3(
                    param["value"][0].GetFloat(), param["value"][1].GetFloat(), param["value"][2].GetFloat()));
            }
            else if (std::string(param["type"].GetString()) == "float4") {
                materialInstance->setParameter(paramName.c_str(), math::float4(
                    param["value"][0].GetFloat(), param["value"][1].GetFloat(), param["value"][2].GetFloat(), param["value"][3].GetFloat()));
            }
        }

        utils::CString materialName(materialJson["materialName"].GetString());
        materialRegistry.registerMaterialInstance(materialName, materialInstance);
        GlobalLog("Registered material instance");
    }

    filamesh::MeshReader::Mesh mesh = filamesh::MeshReader::loadMeshFromFile(engine, utils::Path(wstringToString(getFullPath(stringToWstring(meshURI)))), materialRegistry);
    GlobalLog("Loaded mesh");

    if (mesh.renderable.isNull()) {
        throw std::runtime_error("Failed to load mesh: " + meshURI);
    }

    return mesh.renderable;
}


void applyTransform(filament::Engine* engine, const rapidjson::Value& obj, utils::Entity entity) {
    filament::TransformManager& tcm = engine->getTransformManager();
    GlobalLog("Obtained TransformManager");

    if (!tcm.hasComponent(entity)) {
        tcm.create(entity);
        GlobalLog("Created Transform Component for entity");
    }

    const rapidjson::Value& position = obj["transform"]["position"];
    const rapidjson::Value& rotation = obj["transform"]["rotation"];
    const rapidjson::Value& scale = obj["transform"]["scale"];
    GlobalLog("Obtained transform components");

    // Translation matrix
    math::float3 translationVector = math::float3(position[0].GetFloat(), position[1].GetFloat(), position[2].GetFloat());
    math::mat4f translation = math::mat4f::translation(translationVector);
    GlobalLog("Created translation matrix");

    // Rotation matrix with angle conversion
    float angleDegrees = rotation[0].GetFloat();
    float angleRadians = angleDegrees * (M_PI / 180.0f); // Convert degrees to radians
    float axisX = rotation[1].GetFloat();
    float axisY = rotation[2].GetFloat();
    float axisZ = rotation[3].GetFloat();
    GlobalLog("Converted rotation angle from degrees to radians");

    math::mat4f rotationMat = math::mat4f::rotation(angleRadians, math::float3(axisX, axisY, axisZ));
    GlobalLog("Created rotation matrix");

    // Scale matrix
    math::float3 scaleVector = math::float3(scale[0].GetFloat(), scale[1].GetFloat(), scale[2].GetFloat());
    math::mat4f scaleMat = math::mat4f::scaling(scaleVector);
    GlobalLog("Created scale matrix");

    // Combined transformation matrix
    math::mat4f transform = translation * rotationMat * scaleMat;
    GlobalLog("Created transform matrix");

    auto instance = tcm.getInstance(entity);
    tcm.setTransform(instance, transform);
    GlobalLog("Set transform");

    // TEMPORARY
    for (int row = 0; row < 4; ++row) {
        std::stringstream ss;
        for (int col = 0; col < 4; ++col) {
            ss << transform[row][col] << " ";
        }
		GlobalLog(ss.str());
    }
}


filament::Material* loadMaterial(filament::Engine* engine, const std::string& materialURI) {
    std::vector<uint8_t> materialData = loadFile(stringToWstring(materialURI));
	GlobalLog("Loaded material data");

    if (materialData.empty()) {
        throw std::runtime_error("Failed to load material: " + materialURI);
    }

    filament::Material* material = filament::Material::Builder()
        .package(materialData.data(), materialData.size())
        .build(*engine);
	GlobalLog("Built material");

    if (!material) {
        throw std::runtime_error("Failed to create material from: " + materialURI);
    }
	GlobalLog("Created material");

    return material;
}

std::vector<uint8_t> loadFile(const std::wstring& relativePath) {
    std::ifstream file(getFullPath(relativePath), std::ios::binary);
	GlobalLog("Opened file: " + wstringToString(getFullPath(relativePath)));

    if (!file) {
        throw std::runtime_error("Could not open file: " + wstringToString(relativePath));
    }
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});
}

filament::Texture* loadTexture(filament::Engine* engine, const std::wstring& relativePath) {
    std::vector<uint8_t> fileData = loadFile(relativePath);
	GlobalLog("Loaded file data");

    int width, height, channels;
    unsigned char* data = stbi_load_from_memory(fileData.data(), fileData.size(), &width, &height, &channels, 4);
	GlobalLog("Loaded image data");

    if (!data) {
        throw std::runtime_error("Failed to load image: " + wstringToString(relativePath));
    }
	GlobalLog("Loaded image data successfully");

    filament::Texture* texture = filament::Texture::Builder()
        .width(width)
        .height(height)
        .levels(1)
        .format(filament::Texture::InternalFormat::RGBA8)
        .build(*engine);
	GlobalLog("Built texture");

    filament::Texture::PixelBufferDescriptor buffer(data, size_t(width * height * 4),
        filament::Texture::Format::RGBA, filament::Texture::Type::UBYTE,
        [](void* buffer, size_t, void*) { stbi_image_free(buffer); });
	GlobalLog("Created pixel buffer descriptor");

    texture->setImage(*engine, 0, std::move(buffer));
	GlobalLog("Set image");

    return texture;
}