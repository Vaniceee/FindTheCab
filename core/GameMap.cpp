#include "GameMap.h"
#include "AssetManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// ФУНКЦІЯ ПАРСИНГУ: Зчитує масив цифр (ID плиток) з JSON/TMJ файлу мапи для конкретного шару
std::vector<int> GameMap::ParseCSV(const std::string& content, const std::string& layerName) {
    std::vector<int> result;
    size_t namePos = content.find("\"name\":\"" + layerName + "\"");                          // Шукаємо, де в тексті JSON починається потрібний шар (наприклад "floor")

    if (namePos == std::string::npos) {
        std::cout << "[MAP DEBUG] Layer not found by name: " << layerName << std::endl;
        return result;                                                                          // Якщо шару з такою назвою немає — повертаємо пустий масив
    }

    size_t startLayer = content.rfind("{", namePos);                                            // Знаходимо початок блоку (фігурну дужку) цього шару
    size_t endLayer = content.find("}", namePos);                                              // Знаходимо кінець блоку цього шару, щоб випадково не вилізти в інший шар

    if (startLayer == std::string::npos || endLayer == std::string::npos) return result;

    std::string layerChunks = content.substr(startLayer, endLayer - startLayer);                // Вирізаємо шматок тексту, який стосується суто нашого шару

    size_t dataPos = layerChunks.find("\"data\":[");                                            // Шукаємо всередині цього шару масив "data", де лежать номери плиток

    if (dataPos == std::string::npos) {
        std::cout << "[MAP DEBUG] Array 'data' not found in layer: " << layerName << std::endl;
        return result;
    }
    size_t startIdx = dataPos + 8;                                                              // Зсуваємось на 8 символів вперед, щоб стати точно на першу цифру масиву
    size_t endIdx = layerChunks.find("]", startIdx);                                            // Шукаємо закриту квадратну дужку — кінець масиву з цифрами
    if (endIdx == std::string::npos) return result;

    std::string csv = layerChunks.substr(startIdx, endIdx - startIdx);                          // Витягуємо чистий рядок з цифрами та комами (напр. "1,1,2,0,5...")
    std::stringstream ss(csv);                                                                  // Створюємо потік для зручного розділення рядка по комах
    std::string token;

    // Цикл розбирає рядок по комах і перетворює символи на звичайні змінні int
    while (std::getline(ss, token, ',')) {
        token.erase(remove_if(token.begin(), token.end(), isspace), token.end());               // Видаляємо випадкові пробіли чи переноси рядків навколо цифри
        if (!token.empty()) {
            result.push_back(std::stoi(token));                                                 // Перетворюємо текст "12" в число 12 і кидаємо в наш вектор мапи
        }
    }

    std::cout << "[MAP DEBUG] Layer read successfully '" << layerName << "'. Number of tiles: " << result.size() << std::endl;
    return result;                                                                              // Повертаємо готовий масив чисел для малювання
}


// ФУНКЦІЯ ЗАВАНТАЖЕННЯ: Відкриває файл мапи, задає налаштування та завантажує картинку-тайлсет
bool GameMap::Load(const std::string& mapPath) {
    std::ifstream file(mapPath);                                                                // Відкриваємо файл мапи (.tmj або .json) для зчитування

    if (!file.is_open()) {
        std::cout << "[MAP ERROR] Could not open file: " << mapPath << std::endl;
        return false;                                                                           // Якщо файлу немає за таким шляхом — виводимо помилку
    }

    std::stringstream buffer;
    buffer << file.rdbuf();                                                                     // Закидуємо весь вміст файлу в буфер
    std::string content = buffer.str();                                                         // Перетворюємо буфер у гігантський текстовий рядок
    file.close();                                                                               // Закриваємо файл, він нам більше не потрібен у пам'яті

    // Базові розміри нашої великої розширеної мапи
    mapWidth = 188;                                                                             // Ширина всієї карти в тайлах (наш новий масштаб поверху)
    mapHeight = 261;                                                                            // Висота всієї карти в тайлах
    tileWidth = 16;                                                                             // Розмір однієї плитки в пікселях по ширині (16х16)
    tileHeight = 16;                                                                            // Розмір однієї плитки в пікселях по висоті

    // Запускаємо наш парсер для кожного шару графіки окремо
    floorLayer = ParseCSV(content, "floor");                                                    // Зчитуємо підлогу
    wallLayer = ParseCSV(content, "wall");                                                      // Зчитуємо нижню частину стін
    roofLayer = ParseCSV(content, "roof");                                                      // Зчитуємо дах/верхівки стін (для псевдо-3D ефекту)

    // Очищаємо старі дані перед новим завантаженням (щоб не було дублів чи сміття)
    collisionRects.clear();
    doorTriggers.clear();
    doorNumbers.clear();

    // Зчитуємо векторні об'єкти, які ми намалювали в Tiled
    ParseObjectLayer(content, "collision", collisionRects);                                     // Завантажуємо прямокутники стін (куди гравець врізається)
    ParseObjectLayer(content, "doors", doorTriggers, &doorNumbers);                             // Завантажуємо тригери дверей та їхні номери кабінетів

    tileset = AssetManager::Instance().GetTexture("tileset");                                                 // Завантажуємо .png картинку з плитками через Raylib

    if (tileset.id == 0) {
        std::cout << "[MAP ERROR] Failed to load texture: " << "tileset" << std::endl;
        return false;                                                                           // Якщо картинку не знайдено — ламаємо завантаження
    }

    std::cout << "[MAP SUCCESS] The map was uploaded manually without third-party libraries!" << std::endl;
    return true;                                                                                // Усе пройшло успішно!
}


// ФУНКЦІЯ ОЧИЩЕННЯ: Видаляє карту з пам'яті комп'ютера, коли гра закривається
void GameMap::Unload() {
    floorLayer.clear();                                                                         // Очищаємо масив підлоги
    wallLayer.clear();                                                                          // Очищаємо масив стін
    roofLayer.clear();                                                                          // Очищаємо масив даху
    collisionRects.clear();                                                                     // Очищаємо колізії
    doorTriggers.clear();                                                                       // Очищаємо двері
    doorNumbers.clear();                                                                        // Стмраємо масив номерів дверей
}



// ФУНКЦІЯ МАЛЮВАННЯ ПІД ГРАВЦЕМ: Виводить підлогу, стіни та зелені рамки дверей
void GameMap::DrawBelowPlayer() {
    DrawSingleLayer(floorLayer);                                                                // Малюємо шар підлоги (найнижчий рівень)
    DrawSingleLayer(wallLayer);                                                                 // Малюємо шар стін, на які гравець візуально наступає ногами

    // ТИМЧАСОВИЙ ДЕБАГ: Можна розкоментувати код нижче, щоб побачити червоні стіни-колізії та зелені тригери перед дверями
    //for (const auto& rect : collisionRects) {
    //   DrawRectangleRec(rect, Fade(RED, 0.4f));
    //}
    for (const auto& trigger : doorTriggers) {
        DrawRectangleLinesEx(trigger, 2, GREEN);
    }
}


// ФУНКЦІЯ МАЛЮВАННЯ НАД ГРАВЦЕМ: Малює дах, під який гравець може заходити головою
void GameMap::DrawAbovePlayer() {
    DrawSingleLayer(roofLayer);                                                                 // Виводиться після гравця, створюючи ефект глибини (персонаж ховається під стіну)
}


// ВНУТРІШНЯ ФУНКЦІЯ МАЛЮВАННЯ ШАРУ: Проходить по масиву чисел та виводить плитку за плиткою
void GameMap::DrawSingleLayer(const std::vector<int>& layerData) {
    if (layerData.empty()) return;                                                              // Якщо шар пустий — нічого не малюємо

    int tsCols = tileset.width / tileWidth;                                                     // Рахуємо, скільки всього плиток поміщається в один ряд на .png картинці

    // Подвійний цикл: перебираємо сітку карти по X та Y (координати в тайлах)
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            int idx = x + y * mapWidth;                                                         // Формула переведення двовимірних координат (X, Y) в один довгий індекс масиву
            if (idx >= layerData.size()) continue;

            int globalTileId = layerData[idx];                                                  // Беремо ID плитки в цій клітинці
            if (globalTileId == 0) continue;                                                    // Якщо ID дорівнює 0 — це пусте місце, пропускаємо його

            int tileId = globalTileId - 1;                                                      // В Tiled індекси зміщені на +1, тому віднімаємо 1 для коду C++
            int tsX = (tileId % tsCols) * tileWidth;                                            // Вираховуємо координату X цієї плитки на самій .png картинці
            int tsY = (tileId / tsCols) * tileHeight;                                           // Вираховуємо координату Y цієї плитки на самій .png картинці

            Rectangle srcRec = { (float)tsX, (float)tsY, (float)tileWidth, (float)tileHeight }; // Вирізаємо квадрат 16х16 пікселів з нашого тайлсету
            Vector2 pos = { (float)(x * tileWidth), (float)(y * tileHeight) };                  // Рахуємо, де саме на екрані гри (в пікселях) має стояти ця плитка

            DrawTextureRec(tileset, srcRec, pos, WHITE);                                        // Малюємо цей шматочок текстури у правильній позиції світового простору
        }
    }
}


// ФУНКЦІЯ ПАРСИНГУ ОБ'ЄКТІВ: Витягує з тексту JSON координати векторних прямокутників (стін/дверей)
void GameMap::ParseObjectLayer(const std::string& content, const std::string& layerName, std::vector<Rectangle>& rects, std::vector<std::string>* names) {
    size_t layerPos = content.find("\"name\":\"" + layerName + "\"");                          // Шукаємо блок потрібного шару об'єктів (наприклад "collision")
    if (layerPos == std::string::npos) return;

    size_t objectsPos = content.find("\"objects\":[", layerPos);                                // Шукаємо початок масиву об'єктів "objects" всередині цього шару
    if (objectsPos == std::string::npos) return;

    size_t endObjects = content.find("]", objectsPos);                                          // Шукаємо закриту дужку — кінець списку об'єктів
    std::string objectsText = content.substr(objectsPos, endObjects - objectsPos);              // Вирізаємо чистий текст з усіма об'єктами шару

    size_t objStart = objectsText.find("{");                                                    // Шукаємо початок першого об'єкта (фігурну дужку)

    // Цикл працює доти, доки в тексті знаходяться фігурні дужки нових об'єктів
    while (objStart != std::string::npos) {
        size_t objEnd = objectsText.find("}", objStart);                                        // Шукаємо кінець опису поточного об'єкта
        if (objEnd == std::string::npos) break;
        std::string obj = objectsText.substr(objStart, objEnd - objStart);                      // Текст суто одного об'єкта з його параметрами

        // Лямбда-функція (міні-помічник) для швидкого пошуку і перетворення дробових чисел (x, y, width, height)
        auto getFloat = [&](const std::string& key) -> float {
            size_t pos = obj.find("\"" + key + "\":");
            if (pos == std::string::npos) return 0.0f;
            size_t startValue = pos + key.length() + 3;
            size_t endValue = obj.find_first_of(",}", startValue);
            return std::stof(obj.substr(startValue, endValue - startValue));
        };

        float x = getFloat("x");                                                                // Витягуємо позицію X об'єкта на карті
        float y = getFloat("y");                                                                // Витягуємо позицію Y об'єкта на карті
        float w = getFloat("width");                                                            // Витягуємо ширину прямокутника
        float h = getFloat("height");                                                           // Витягуємо висоту прямокутника

        rects.push_back({ x, y, w, h });                                                        // Створюємо Rectangle з цими даними і додаємо у список колізій/тригерів

        // Якщо нам передали вказівник на масив імен (значить це двері) — витягуємо кастомну властивість номера кабінету
        if (names) {
            size_t valPos = obj.find("\"name\":\"");                                           // Шукаємо рядок "value":"номер_кабінету"
            if (valPos != std::string::npos) {
                size_t valStart = valPos + 8;
                size_t valEnd = obj.find("\"", valStart);
                names->push_back(obj.substr(valStart, valEnd - valStart));                      // Зберігаємо назву кабінету (наприклад "216") у список
            }
            else {
                names->push_back("unknown");                                                    // Якщо властивість забули написати в Tiled — ставимо "unknown"
            }
        }
        objStart = objectsText.find("{", objEnd);                                               // Переходимо до пошуку наступного об'єкта в тексті
    }

    std::cout << "[DEBUG] Object layer '" << layerName << "' rectangles loded: " << rects.size() << std::endl;
    if (names) {
        std::cout << "[DEBUG] For '" << layerName << "' readed names/nombers: " << names->size() << std::endl;
    }
}


// ФУНКЦІЯ ПЕРЕВІРКИ ЗІТКНЕНЬ: Перевіряє, чи не наступив гравець своїм хітбоксом на якусь стіну
bool GameMap::CheckWallCollision(Rectangle playerHitbox) {
    // Перебираємо абсолютно всі прямокутники колізій стін, які ми розпарсили раніше
    for (const auto& rect : collisionRects) {
        if (CheckCollisionRecs(playerHitbox, rect)) {                                           // Функція Raylib перевіряє перетин двох прямокутників
            return true;                                                                        // Якщо перетнулися — повертаємо true (стіна! йти не можна!)
        }
    }
    return false;                                                                               // Якщо пройшли весь список і зачеплень немає — повертаємо false (шлях вільний)
}


void GameMap::UpdateDoorTriggers(Rectangle playerHitbox) {
    // Кожного кадру спочатку очищаємо повідомлення
    currentDoorMessage = "";
    // Циклом перевіряємо колізію з кожним прямокутником дверей
    for (size_t i = 0; i < doorTriggers.size(); ++i) {
        if (CheckCollisionRecs(playerHitbox, doorTriggers[i])) {
          // Якщо доторкнулися, витягуємо її Name (який ти вписала англійською в Tiled)
            std::string doorName = "unknown";
            if (i < doorNumbers.size()) {
                doorName = doorNumbers[i];
            }

            // Склеюємо красивий рядок-підказку
            currentDoorMessage = "Press 'E' to enter: " + doorName;
            break; // Знайшли перші двері, біля яких стоїмо — виходимо з циклу
        }
    }
}

