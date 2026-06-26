#include "AssetManager.h"
#include <iostream>

AssetManager& AssetManager::Instance() {
    static AssetManager instance;
    return instance;
}

void AssetManager::LoadTexture(const std::string& name, const std::string& path) {
    if (textures.find(name) == textures.end()) {
        // Додали :: перед LoadTexture, щоб викликати функцію з Raylib
        Texture2D tex = ::LoadTexture(path.c_str());

        if (tex.id > 0) {
            textures[name] = tex;
        }
        else {
            std::cout << "ПОМИЛКА: Не вдалося завантажити текстуру: " << path << std::endl;
        }
    }
}

Texture2D AssetManager::GetTexture(const std::string& name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second;
    }
    return Texture2D{ 0 };
}

void AssetManager::UnloadAll() {
    // ЗАМІНЕНО цикл для сумісності з усіма версіями C++
    for (std::map<std::string, Texture2D>::iterator it = textures.begin(); it != textures.end(); ++it) {
        ::UnloadTexture(it->second);
    }
    textures.clear();
}