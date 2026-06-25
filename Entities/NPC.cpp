#include "NPC.h"
#include "raymath.h"

NPC::NPC(Vector2 pos, const std::string& text, Texture2D* sharedTexture)
{
    position = pos;
    dialogue = text;
    color = WHITE;

    // Просто запам'ятовуємо адресу спільної текстури
    texture = sharedTexture;

    currentFrame = 0;
    frameTimer = 0.0f;
    frameDuration = 0.14f;
}

NPC::~NPC()
{
    // Тут порожньо! Текстура вивантажується один раз в main.cpp
}

void NPC::Update()
{
    // Оновлення таймера анімації NPC
    frameTimer += GetFrameTime();
    if (frameTimer >= frameDuration) {
        frameTimer = 0.0f;
        currentFrame++;
        if (currentFrame >= 4) {
            currentFrame = 0;
        }
    }
}

void NPC::Draw()
{
    // Перевірка на випадок, якщо вказівник нульовий
    if (texture == nullptr) return;

    float sourceX = currentFrame * spriteWidth;
    float sourceY = 0.0f * spriteHeight;

    Rectangle sourceRec = { sourceX, sourceY, spriteWidth, spriteHeight };
    Rectangle destRec = { position.x, position.y, spriteWidth * scale, spriteHeight * scale };

    // Розархівовуємо вказівник (*texture), щоб передати чисту текстуру в Raylib
    DrawTexturePro(*texture, sourceRec, destRec, { 0.0f, 0.0f }, 0.0f, WHITE);
}

bool NPC::IsPlayerNear(Vector2 playerPos, float range)
{
    return Vector2Distance(position, playerPos) < range;
}