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
    ~NPC(); 

    void Draw();
    bool IsPlayerNear(Vector2 playerPos, float range = 60.0f); 

private:
    Texture2D texture;      
    const float scale = 3.0f; 
};