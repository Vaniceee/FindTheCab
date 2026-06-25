#pragma once
#include "Entities.h" // Теж підключаємо Character
#include <string>

// NPC стає нащадком класу Character
class NPC : public Character {
public:
    std::string dialogue;
    Color color;

    NPC(Vector2 pos, const std::string& text, Texture2D* sharedTexture);
    ~NPC();

    void Update();
    void Draw();
    bool IsPlayerNear(Vector2 playerPos, float range = 30.0f);

private:
    Texture2D* texture; // Вказівник на спільну текстуру (керується ззовні)
};