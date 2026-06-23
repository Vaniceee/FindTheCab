#pragma once
#include "raylib.h"
#include <string>

class NPC
{
public:
    Vector2 position;
    std::string dialogue;
    Color color;

    // Конструктор приймає вказівник на спільну текстуру, завантажену в main.cpp
    NPC(Vector2 pos, const std::string& text, Texture2D* sharedTexture);
    ~NPC();

    void Update();
    void Draw();
    bool IsPlayerNear(Vector2 playerPos, float range = 30.0f);

private:
    Texture2D* texture; // Вказівник на текстуру (клас сам нею не керує)

    const float spriteWidth = 24.0f;
    const float spriteHeight = 24.0f;
    const float scale = 1.2f; // Масштаб спрайту у світі

    int currentFrame;
    float frameTimer;
    float frameDuration;
};