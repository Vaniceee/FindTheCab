#include "GameMap.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// Допоміжна функція для пошуку масиву цифр у тексті JSON/TMJ
std::vector<int> GameMap::ParseCSV(const std::string& content, const std::string& layerName) {
    std::vector<int> result;

    // 1. Шукаємо назву шару
    size_t namePos = content.find("\"name\":\"" + layerName + "\"");
    if (namePos == std::string::npos) {
        std::cout << "[MAP DEBUG] Шар не знайдено за назвою: " << layerName << std::endl;
        return result;
    }

    // 2. Знаходимо фігурні дужки саме цього шару, щоб не вилізти за його межі
    size_t startLayer = content.rfind("{", namePos);
    size_t endLayer = content.find("}", namePos);
    if (startLayer == std::string::npos || endLayer == std::string::npos) return result;

    std::string layerChunks = content.substr(startLayer, endLayer - startLayer);

    // 3. Шукаємо масив data конкретно в цьому шарі
    size_t dataPos = layerChunks.find("\"data\":[");
    if (dataPos == std::string::npos) {
        std::cout << "[MAP DEBUG] Масив 'data' не знайдено у шарі: " << layerName << std::endl;
        return result;
    }

    size_t startIdx = dataPos + 8; // Довжина "\"data\":["
    size_t endIdx = layerChunks.find("]", startIdx);
    if (endIdx == std::string::npos) return result;

    std::string csv = layerChunks.substr(startIdx, endIdx - startIdx);
    std::stringstream ss(csv);
    std::string token;

    while (std::getline(ss, token, ',')) {
        // Прибираємо можливі пробіли чи переноси рядків
        token.erase(remove_if(token.begin(), token.end(), isspace), token.end());
        if (!token.empty()) {
            result.push_back(std::stoi(token));
        }
    }

    std::cout << "[MAP DEBUG] Успішно зчитано шар '" << layerName << "'. Кількість тайлів: " << result.size() << std::endl;
    return result;
}

bool GameMap::Load(const std::string& mapPath, const std::string& texturePath) {
    std::ifstream file(mapPath);
    if (!file.is_open()) {
        std::cout << "[MAP ERROR] Не вдалося відкрити файл: " << mapPath << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();

    // Задаємо жорсткі розміри з твого файлу мапи
    mapWidth = 150;
    mapHeight = 150;
    tileWidth = 16;
    tileHeight = 16;

    // Парсимо наші шари
    floorLayer = ParseCSV(content, "floor");
    wallLayer = ParseCSV(content, "wall");
    roofLayer = ParseCSV(content, "roof");

    collisionRects.clear();
    doorTriggers.clear();
    doorNumbers.clear();

    ParseObjectLayer(content, "collision", collisionRects);
    ParseObjectLayer(content, "doors", doorTriggers, &doorNumbers);

    tileset = LoadTexture(texturePath.c_str());
    if (tileset.id == 0) {
        std::cout << "[MAP ERROR] Не вдалося завантажити текстуру: " << texturePath << std::endl;
        return false;
    }

    std::cout << "[MAP SUCCESS] Мапа завантажена власноруч без сторонніх бібліотек!" << std::endl;
    return true;
}

void GameMap::Unload() {
    if (tileset.id != 0) {
        UnloadTexture(tileset);
        tileset.id = 0;
    }
    floorLayer.clear();
    wallLayer.clear();
    roofLayer.clear();
}

void GameMap::DrawBelowPlayer() {
    DrawSingleLayer(floorLayer);
    DrawSingleLayer(wallLayer);

    // ТИМЧАСОВИЙ ДЕБАГ: Малюємо червоні колізії, щоб побачити їх
   // for (const auto& rect : collisionRects) {
     //   DrawRectangleRec(rect, Fade(RED, 0.4f));
    //}

    // ТИМЧАСОВИЙ ДЕБАГ: Малюємо сині тригери дверей
    for (const auto& rect : doorTriggers) {
        DrawRectangleRec(rect, Fade(BLUE, 0.4f));
    }
}

void GameMap::DrawAbovePlayer() {
    DrawSingleLayer(roofLayer);
}

void GameMap::DrawSingleLayer(const std::vector<int>& layerData) {
    if (layerData.empty()) return;

    int tsCols = tileset.width / tileWidth;

    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            int idx = x + y * mapWidth;
            if (idx >= layerData.size()) continue;

            int globalTileId = layerData[idx];
            if (globalTileId == 0) continue; // Порожньо

            int tileId = globalTileId - 1;
            int tsX = (tileId % tsCols) * tileWidth;
            int tsY = (tileId / tsCols) * tileHeight;

            Rectangle srcRec = { (float)tsX, (float)tsY, (float)tileWidth, (float)tileHeight };
            Vector2 pos = { (float)(x * tileWidth), (float)(y * tileHeight) };

            DrawTextureRec(tileset, srcRec, pos, WHITE);
        }
    }
}

void GameMap::ParseObjectLayer(const std::string& content, const std::string& layerName, std::vector<Rectangle>& rects, std::vector<std::string>* names) {
    size_t layerPos = content.find("\"name\":\"" + layerName + "\"");
    if (layerPos == std::string::npos) return;

    size_t objectsPos = content.find("\"objects\":[", layerPos);
    if (objectsPos == std::string::npos) return;

    size_t endObjects = content.find("]", objectsPos);
    std::string objectsText = content.substr(objectsPos, endObjects - objectsPos);

    // Шукаємо кожен об'єкт {} у масиві
    size_t objStart = objectsText.find("{");
    while (objStart != std::string::npos) {
        size_t objEnd = objectsText.find("}", objStart);
        if (objEnd == std::string::npos) break;

        std::string obj = objectsText.substr(objStart, objEnd - objStart);

        // Витягуємо координати та розміри
        auto getFloat = [&](const std::string& key) -> float {
            size_t pos = obj.find("\"" + key + "\":");
            if (pos == std::string::npos) return 0.0f;
            size_t startValue = pos + key.length() + 3;
            size_t endValue = obj.find_first_of(",}", startValue);
            return std::stof(obj.substr(startValue, endValue - startValue));
            };

        float x = getFloat("x");
        float y = getFloat("y");
        float w = getFloat("width");
        float h = getFloat("height");

        rects.push_back({ x, y, w, h });

        // Якщо шар — це двері, витягуємо їхній номер (value)
        if (names) {
            size_t valPos = obj.find("\"value\":\"");
            if (valPos != std::string::npos) {
                size_t valStart = valPos + 9;
                size_t valEnd = obj.find("\"", valStart);
                names->push_back(obj.substr(valStart, valEnd - valStart));
            }
            else {
                names->push_back("unknown");
            }
        }

        objStart = objectsText.find("{", objEnd);
    }

}
    
    bool GameMap::CheckWallCollision(Rectangle playerHitbox) {
        for (const auto& rect : collisionRects) {
            if (CheckCollisionRecs(playerHitbox, rect)) {
                return true;
            }
        }
        return false;
    }



