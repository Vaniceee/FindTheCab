#include "NPC.h"
#include "raymath.h"

NPC::NPC(Vector2 pos, const std::string& text)
{
    position = pos;
    dialogue = text;
    color = WHITE; 

    
    texture = LoadTexture("assets/16x16 Idle.png");
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);
}

NPC::~NPC()
{
   
    UnloadTexture(texture);
}

void NPC::Draw()
{

    float frameSize = (float)texture.height;

   
    Rectangle sourceRec = { 0.0f, 0.0f, frameSize, frameSize };


    Rectangle destRec = {
        position.x,
        position.y,
        frameSize * scale,
        frameSize * scale
    };

   
    DrawTexturePro(texture, sourceRec, destRec, { 0.0f, 0.0f }, 0.0f, WHITE);
}

bool NPC::IsPlayerNear(Vector2 playerPos, float range)
{
    return Vector2Distance(position, playerPos) < range;
}