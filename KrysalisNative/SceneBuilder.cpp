// SceneBuilder.cpp
// scene loading and updating

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
#include <rapidjson/document.h>   // For parsing JSON
#include <rapidjson/istreamwrapper.h>  // For handling input stream to JSON
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream> // deduplicate
#include <stb_image.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace filament;

// auto sort and format cpp files?

void updateScene() {
    // Update the scene from changesets
}

void loadScene() {
    loadSceneFromFile("scenes\\scene.json"); // load from file since we don't have dynamic loading yet
}


rapidjson::Document loadSceneFromFile(const std::string& relativePath) {
    // Open the file using ifstream
    std::ifstream file(wstringToString(getFullPath(stringToWstring(relativePath))));

    // Check if the file is open and valid
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + relativePath);
    }

    // Use IStreamWrapper to read the file into a RapidJSON document
    rapidjson::IStreamWrapper isw(file);
    rapidjson::Document sceneData;
    sceneData.ParseStream(isw);  // Parse the stream directly into the JSON Document

    // Check for parsing errors
    if (sceneData.HasParseError()) {
        throw std::runtime_error("Error parsing JSON file: " + relativePath);
    }

    // Return the parsed document (which is the JSON object in RapidJSON)
    return sceneData;
}


void createScene(filament::Engine* engine, filament::Scene* scene, const rapidjson::Document& sceneData) {
    utils::EntityManager& em = utils::EntityManager::get();

    const rapidjson::Value& objects = sceneData["objects"];
    for (rapidjson::SizeType i = 0; i < objects.Size(); i++) {
        const rapidjson::Value& obj = objects[i];

        // Create a new entity
        utils::Entity entity = em.create();
        std::string type = obj["type"].GetString();

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


filament::LightManager::Builder createLightComponent(const rapidjson::Value& obj) {
    filament::LightManager::Builder builder(
        std::string(obj["component"]["lightType"].GetString()) == "directional"
        ? filament::LightManager::Type::DIRECTIONAL
        : filament::LightManager::Type::POINT);

    const rapidjson::Value& color = obj["component"]["color"];
    builder.color({ color[0].GetFloat(), color[1].GetFloat(), color[2].GetFloat() })
        .intensity(obj["component"]["intensity"].GetFloat());

    if (std::string(obj["component"]["lightType"].GetString()) == "point") {
        builder.falloff(obj["component"]["falloff"].GetFloat());
    }

    if (obj["component"].HasMember("castShadows") && obj["component"]["castShadows"].GetBool()) {
        builder.castShadows(true);
    }

    return builder;
}


void createMeshComponent(filament::Engine* engine, filament::Scene* scene, const rapidjson::Value& obj, utils::Entity entity) {
    std::string meshURI = obj["component"]["meshURI"].GetString();

    // Load the mesh (use a function to handle this)
    filament::VertexBuffer* vb;
    filament::IndexBuffer* ib;
    loadMeshFromFile(engine, meshURI, &vb, &ib);

    // Create material instances from the JSON
    const rapidjson::Value& materials = obj["component"]["materials"];
    for (rapidjson::SizeType i = 0; i < materials.Size(); i++) {
        const rapidjson::Value& materialJson = materials[i];
        std::string materialURI = materialJson["materialURI"].GetString();

        filament::Material* material = loadMaterial(engine, materialURI);
        filament::MaterialInstance* materialInstance = material->createInstance();

        // Apply parameters
        const rapidjson::Value& parameters = materialJson["parameters"];
        for (rapidjson::SizeType j = 0; j < parameters.Size(); j++) {
            const rapidjson::Value& param = parameters[j];
            std::string paramName = param["name"].GetString();

            if (std::string(param["type"].GetString()) == "sampler2d") {
                filament::Texture* texture = loadTexture(engine, stringToWstring(param["value"].GetString()));
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

        // Build the renderable component
        filament::RenderableManager::Builder(1)
            .boundingBox({ {0, 0, 0}, {1, 1, 1} })
            .geometry(0, filament::RenderableManager::PrimitiveType::TRIANGLES, vb, ib)
            .material(0, materialInstance)
            .build(*engine, entity);
    }
}


void applyTransform(filament::Engine* engine, const rapidjson::Value& obj, utils::Entity entity) {
    filament::TransformManager& tcm = engine->getTransformManager();

    const rapidjson::Value& position = obj["transform"]["position"];
    const rapidjson::Value& rotation = obj["transform"]["rotation"];
    const rapidjson::Value& scale = obj["transform"]["scale"];

    math::float3 translationVector = math::float3(position[0].GetFloat(), position[1].GetFloat(), position[2].GetFloat());
    math::mat4f translation = math::mat4f::translation(translationVector);

    float angle = rotation[0].GetFloat();
    float axisX = rotation[1].GetFloat(); 
    float axisY = rotation[2].GetFloat();
    float axisZ = rotation[3].GetFloat();


    // Create the rotation matrix from the axis-angle
    math::mat4f rotationMat = math::mat4f::rotation(angle, math::float3(axisX, axisY, axisZ));

	math::float3 scaleVector = math::float3(scale[0].GetFloat(), scale[1].GetFloat(), scale[2].GetFloat());
	math::mat4f scaleMat = math::mat4f::scaling(scaleVector);

    // Combine translation, rotation, and scale into one transform matrix
    math::mat4f transform = translation * rotationMat * scaleMat;

    tcm.setTransform(tcm.getInstance(entity), transform);
}


void loadMeshFromFile(filament::Engine* engine, const std::string& meshURI, filament::VertexBuffer** vb, filament::IndexBuffer** ib) {
    // Convert meshURI to a wide string if needed and get the full path
    std::wstring fullPath = getFullPath(stringToWstring(meshURI));

    // Load the mesh from the file
    filamesh::MeshReader::Mesh mesh = filamesh::MeshReader::loadMeshFromFile(engine, utils::Path(wstringToString(fullPath)));

    if (mesh.renderable.isNull()) {
        throw std::runtime_error("Failed to load mesh: " + meshURI);
    }

    // Extract the vertex and index buffers from the mesh
    *vb = mesh.vertexBuffer;
    *ib = mesh.indexBuffer;
}

filament::Material* loadMaterial(filament::Engine* engine, const std::string& materialURI) {
    // Convert the materialURI to a wide string and get the full path
    std::wstring fullPath = getFullPath(stringToWstring(materialURI));

    // Load the material file data
    std::vector<uint8_t> materialData = loadFile(fullPath);
    if (materialData.empty()) {
        throw std::runtime_error("Failed to load material: " + materialURI);
    }

    // Create and build the material from the package
    filament::Material* material = filament::Material::Builder()
        .package(materialData.data(), materialData.size())
        .build(*engine);

    if (!material) {
        throw std::runtime_error("Failed to create material from: " + materialURI);
    }

    return material;
}

std::vector<uint8_t> loadFile(const std::wstring& relativePath) {
    std::ifstream file(getFullPath(relativePath), std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open file: " + wstringToString(relativePath));
    }
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});
}

filament::Texture* loadTexture(filament::Engine* engine, const std::wstring& relativePath) {
    std::vector<uint8_t> fileData = loadFile(relativePath);

    int width, height, channels;
    unsigned char* data = stbi_load_from_memory(fileData.data(), fileData.size(), &width, &height, &channels, 4);

    if (!data) {
        throw std::runtime_error("Failed to load image: " + wstringToString(relativePath));
    }

    filament::Texture* texture = filament::Texture::Builder()
        .width(width)
        .height(height)
        .levels(1)
        .format(filament::Texture::InternalFormat::RGBA8)
        .build(*engine);

    filament::Texture::PixelBufferDescriptor buffer(data, size_t(width * height * 4),
        filament::Texture::Format::RGBA, filament::Texture::Type::UBYTE,
        [](void* buffer, size_t, void*) { stbi_image_free(buffer); });

    texture->setImage(*engine, 0, std::move(buffer));

    return texture;
}