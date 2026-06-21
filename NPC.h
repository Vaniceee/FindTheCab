#pragma once
#include "raylib.h"
#include <string>

class NPC
{
public:
    Vector2 position;
    std::string dialogue;
    Color color;

    NPC(Vector2 pos, const std::string& text);

    void Draw();
    bool IsPlayerNear(Vector2 playerPos, float range = 50.0f);
};