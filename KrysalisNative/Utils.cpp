// Utils.cpp
#include <filament/Engine.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/Texture.h>
#include <utils/Path.h>
#include <fstream>
#include <vector>
#include <stb_image.h>
#include <iostream>
#include <string>

std::vector<uint8_t> loadFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open file: " + filePath);
    }
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});
}

filament::Texture* loadTexture(filament::Engine* engine, const std::string& filePath) {
    // Load file data into memory
    std::vector<uint8_t> fileData = loadFile(filePath);

    // Decode the image using stb_image
    int width, height, channels;
    unsigned char* data = stbi_load_from_memory(fileData.data(), fileData.size(), &width, &height, &channels, 4); // 4 means RGBA

    if (!data) {
        throw std::runtime_error("Failed to load image: " + filePath);
    }

    // Create the Filament Texture
    filament::Texture* texture = filament::Texture::Builder()
        .width(width)
        .height(height)
        .levels(1) // No mipmaps for simplicity, but you can change this
        .format(filament::Texture::InternalFormat::RGBA8) // RGBA format
        .build(*engine);

    // Set the pixel data in the texture
    filament::Texture::PixelBufferDescriptor buffer(data, size_t(width * height * 4),
        filament::Texture::Format::RGBA, filament::Texture::Type::UBYTE,
        [](void* buffer, size_t, void*) { stbi_image_free(buffer); });

    texture->setImage(*engine, 0, std::move(buffer));

    return texture;
}