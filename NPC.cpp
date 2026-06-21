#include "NPC.h"
#include "raymath.h"

NPC::NPC(Vector2 pos, const std::string& text)
{
    position = pos;
    dialogue = text;
    color = RED;
}

void NPC::Draw()
{
    DrawCircleV(position, 10, color);
}

bool NPC::IsPlayerNear(Vector2 playerPos, float range)
{
    return Vector2Distance(position, playerPos) < range;
}