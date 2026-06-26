#pragma once
#include "Entities.h" // Підключаємо нашого базового "батька"

enum class PlayerDirection {                              // Напрямки руху персонажа для коректної анімації
    DOWN = 0,        
    DOWN_RIGHT = 1,  
    RIGHT = 2,       
    UP_RIGHT = 3,    
    UP = 4           
};

enum class PlayerState {                                 // Стани персонажа
    IDLE,
    WALKING
};

class Player : public Character {                       // Клас гравця, успадковує базові параметри від Character
public:
    Player();
    Player(Vector2 spawnPosition);
    ~Player();

    void Load();
    void Update(int windowWidth, int windowHeight);
    void Draw();

    // Геттери для доступу до параметрів гравця з інших модулів
    float GetStamina() const { return stamina; }        // Передача стаміни в GameManager/UI
    bool IsSprinting() const;

private:
    float baseSpeed;

    // Стаміна і втома (коли блокується shift)
    float stamina;
    bool isExhausted;

    PlayerDirection direction;
    PlayerState state;
    bool flipX;

    // Текстури гравця (мають бути вивантажені через Unload)
    Texture2D textureIdle;
    Texture2D textureWalk;
    Texture2D* currentTexture;                          // Вказівник на поточну активну текстуру
};