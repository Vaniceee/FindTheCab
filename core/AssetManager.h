#pragma once
#include "raylib.h"
#include <string>
#include <map>

class AssetManager {
public:
    static AssetManager& Instance();                                            // Повертає єдиний екземпляр менеджера (Singleton)
    void LoadTexture(const std::string& name, const std::string& path);         // Завантаження текстури та збереження її під іменем 'name'
    Texture2D GetTexture(const std::string& name);                              // Отримання вже завантаженої текстури
    void UnloadAll();                                                           // Очищення всієї пам'яті (викликати при закритті гри)

private:
    AssetManager() {}                                                           // Приватний конструктор, щоб ніхто не створив інший менеджер
    std::map<std::string, Texture2D> textures;                                  // Словник для зберігання текстур
};
