#pragma once
#include "raylib.h"
#include <string>
#include <vector>

class GameMap { 
public:
    GameMap() : mapWidth(0), mapHeight(0), tileWidth(16), tileHeight(16) {}                     // "Констуктор" (спец.функція) яка задає базові налаштування для змінних у класах:
                                                                                                // Ширина та висота мапи в тайлах; ширина та висота тайлів в пікселях

    ~GameMap() { Unload(); }                                                                    // ~ Означає деструктор - коли гра закривається то пк автоматично шукає цю функцію
                                                                                                // щоб правильно видалити мапу і очистити пам'ять
                                                                                                // без цього рядку буде витік пам'яті, бо графіка і аудіо не видаляється автоматично

    bool CheckWallCollision(Rectangle playerHitbox);                                            // Дивимось чи стикається гравець з колізією стін (передаємо хітбокс в ф-цію)
    bool Load(const std::string& mapPath, const std::string& texturePath);                      // Ф-ція яка опрацьовує tmj файл мапи, так як у нас свій парсер для неї
    void DrawBelowPlayer();                                                                     // Ф-ція виводить шари мапи на які гравець може заходити (підлога, стіни до колізії)
    void DrawAbovePlayer();                                                                     // Ф-ція малює те що над гравцем (частина стіни яка накриває персонажа для 3-д ефекту)
    void Unload();                                                                              // Ф-ція очищення карти (яку ми використали в деструкторі зверху)

    std::string currentDoorMessage = "";                                                        // Змінна, яка зберігатиме текст підказки біля дверей

    void UpdateDoorTriggers(Rectangle playerHitbox);                                            // Функція, яка щокадру перевіряє, чи стоїмо ми біля якихось дверей

private:
    int mapWidth;                                                                               // Ширина всієї карти в тайлах (за зам. = 0)
    int mapHeight;                                                                              // Висота всієї карти в тайлах (за зам. = 0)
    int tileWidth;                                                                              // Ширина самого тайла в пікселях (за зам. = 16 і без змін)
    int tileHeight;                                                                             // Висота самого тайла в пікселях (за зам. - 16 і без змін)

    Texture2D tileset;                                                                          // Змінна типу Texture2D з raylib, яка зберігає pdf файл з плитками
                                                                                                // Без цього мапа буде просто масивом id і не намалюється

    std::vector<int> floorLayer;                                                                // Зберігає масив id всіх плиток шару підлоги
    std::vector<int> wallLayer;                                                                 // Зберігає масив id всіх плиток шару стін на які заходить гравець
    std::vector<int> roofLayer;                                                                 // Зберігає масив id всіх плиток шару стін, які накривають гравця зверху
    
    void DrawSingleLayer(const std::vector<int>& layerData);                                    // Ф-ція яка малює якийсь шар карти
    std::vector<int> ParseCSV(const std::string& fileContent, const std::string& layerName);    // Ф-ція зчитує .tmj файл мапи і виписує потрібний шар у чистий масив С++

    std::vector<Rectangle> collisionRects;                                                      // Масив прямокутників колізій для стін
    std::vector<Rectangle> doorTriggers;                                                        // Масив прямокутних трігерів для дверей
    std::vector<std::string> doorNumbers;                                                       // Масив властивостей дверей (номери дверей)

    // Ф-ція зчитує з .tmj координати прямокутників об'єктів (стін і дверей) та за необхідності власні коди/властивості (пр. номери дверей)
    void ParseObjectLayer(const std::string& content, const std::string& layerName, std::vector<Rectangle>& rects, std::vector<std::string>* names = nullptr);
};