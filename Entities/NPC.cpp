#include "NPC.h"
#include "raymath.h"

NPC::NPC(Vector2 pos, const std::string& text, Texture2D* sharedTexture) {
    position = pos;
    dialogue = text;
    color = WHITE;
    texture = sharedTexture;

    // Ініціалізуємо змінні Character під кастомний піксель-арт NPC
    spriteWidth = 24.0f;
    spriteHeight = 24.0f;
    scale = 1.2f;

    currentFrame = 0;
    frameTimer = 0.0f;
    frameDuration = 0.14f;
}

NPC::~NPC() {
    // Порожньо, текстура вивантажується в GameManager/main
}

void NPC::Update() {
    // Оновлення таймера анімації NPC (працює на батьківських змінних)
    frameTimer += GetFrameTime();
    if (frameTimer >= frameDuration) {
        frameTimer = 0.0f;
        currentFrame++;
        if (currentFrame >= 4) {
            currentFrame = 0;
        }
    }
}

void NPC::Draw() {
    if (texture == nullptr) return;

    float sourceX = currentFrame * spriteWidth;
    float sourceY = 0.0f * spriteHeight; // Рядок 0 на спрайт-шиті NPC

    Rectangle sourceRec = { sourceX, sourceY, spriteWidth, spriteHeight };
    Rectangle destRec = { position.x, position.y, spriteWidth * scale, spriteHeight * scale };

    DrawTexturePro(*texture, sourceRec, destRec, { 0.0f, 0.0f }, 0.0f, WHITE);

    // Можна також увімкнути зелену/синю лінію хитбоксу для тесту фізики NPC:
    // DrawRectangleLinesEx(Character::GetFeetHitbox(), 1.0f, BLUE);
}

bool NPC::IsPlayerNear(Vector2 playerPos, float range) {
    return Vector2Distance(position, playerPos) < range;
}