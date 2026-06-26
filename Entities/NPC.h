#pragma once
#include "Entities.h" // Теж підключаємо Character
#include <string>

// NPC — це спеціалізований персонаж, який може розмовляти з гравцем
class NPC : public Character {
public:
    std::string dialogue;           // NPC — це спеціалізований персонаж, який може розмовляти з гравцем
    Color color;                    // Колір NPC (наприклад, для виділення викладачів)

    // Конструктор: позиція, текст діалогу та вказівник на текстуру
    NPC(Vector2 pos, const std::string& text, Texture2D* sharedTexture);
    ~NPC();                                                     // Деструктор для очищення ресурсів

    void Update();                                              // Оновлення логіки (наприклад, анімація)
    void Draw();                                                // Малювання персонажа на екрані
    bool IsPlayerNear(Vector2 playerPos, float range = 30.0f);  // Перевірка, чи знаходиться гравець у зоні взаємодії

private:
    Texture2D* texture;                                         // Вказівник на спільну текстуру (керується ззовні)
};