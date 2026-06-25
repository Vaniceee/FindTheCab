#pragma once
#include "raylib.h"

// Базовий клас для будь-якої живої істоти у світі гри (Гравець, NPC, викладачі)
class Character {
protected:
    Vector2 position;         // Позиція персонажа на мапі
    float currentSpeed;       // Поточна швидкість пересування

    // Графічні константи (у кожного сина можуть бути свої, тому робимо protected)
    float spriteWidth;
    float spriteHeight;
    float scale;

    // Спільні змінні для покадрової піксельної анімації
    int currentFrame;
    float frameTimer;
    float frameDuration;

public:
    // Конструктор за замовчуванням
    Character() :
        position{ 0.0f, 0.0f }, currentSpeed{ 0.0f },
        spriteWidth{ 32.0f }, spriteHeight{ 32.0f }, scale{ 1.0f },
        currentFrame{ 0 }, frameTimer{ 0.0f }, frameDuration{ 0.14f } {}

    // Геттери та сеттери для позиції
    Vector2 GetPosition() const { return position; }
    void SetPosition(Vector2 pos) { position = pos; }

    // Універсальний хитбокс ніг персонажа для перевірки колізій зі стінами
    Rectangle GetFeetHitbox() const {
        // Завдяки protected змінним, цей метод автоматично працюватиме і для NPC, і для Player
        return Rectangle{
            position.x + (8.0f * scale),
            position.y + (24.0f * scale),
            16.0f * scale,
            8.0f * scale
        };
    }
};