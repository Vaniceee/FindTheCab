//#pragma once
//#include "raylib.h"
//class GameMap {
//public:
//	static const int MAP_WIDTH = 40;
//	static const int MAP_HEIGHT = 40;
//	static const int TILE_SIZE = 16;
//	static const float TILE_SCALE;
//
//
//	GameMap();
//	~GameMap();
//
//	void Draw();
//	float GetWorldWidth() const { return MAP_WIDTH * TILE_SIZE * TILE_SCALE; }
//	float GetWorldHeight() const { return MAP_HEIGHT * TILE_SIZE * TILE_SCALE; }
//
//private:
//	Texture2D tilesetTexture;
//	int floorLayer[MAP_HEIGHT][MAP_WIDTH];
//
//
//
//
//
//
//};
#pragma once
#include "raylib.h"
#include <string>
#include <vector>

class GameMap {
public:
    GameMap() : mapWidth(0), mapHeight(0), tileWidth(16), tileHeight(16) {}
    ~GameMap() { Unload(); }

    bool CheckWallCollision(Rectangle playerHitbox);
    bool Load(const std::string& mapPath, const std::string& texturePath);
    void DrawBelowPlayer();
    void DrawAbovePlayer();
    void Unload();

private:
    int mapWidth;
    int mapHeight;
    int tileWidth;
    int tileHeight;

    Texture2D tileset;

    // Сховища для чистих ID тайлів
    std::vector<int> floorLayer;
    std::vector<int> wallLayer;
    std::vector<int> roofLayer;
    void DrawSingleLayer(const std::vector<int>& layerData);
    std::vector<int> ParseCSV(const std::string& fileContent, const std::string& layerName);

    // ... ваші минулі змінні (floorLayer, wallLayer тощо) ...

// Шари об'єктів (колізії та двері)
    std::vector<Rectangle> collisionRects;
    std::vector<Rectangle> doorTriggers;
    std::vector<std::string> doorNumbers; // Зберігатиме номери дверей (наприклад, "216")

    // Метод для зчитування прямокутників з тексту
    void ParseObjectLayer(const std::string& content, const std::string& layerName, std::vector<Rectangle>& rects, std::vector<std::string>* names = nullptr);
};