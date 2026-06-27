#pragma once
#include "raylib.h"

class StaminaBar {                                                  // Клас для керування та відображення шкали стаміни
public:
    StaminaBar();                                                   // Конструктор: ініціалізує початковий стан
    ~StaminaBar() {}                                                 // Деструктор: гарантує звільнення ресурсів

    void Initialize();                                                    // Завантажує текстури шкали у відеопам'ять
    void Draw(float currentStamina, Vector2 position);              // Малює шкалу на екрані згідно з поточним значенням стаміни

private:
    static const int STAMINA_FRAMES = 11;                           // Кількість кадрів анімації шкали
    Texture2D staminaTextures[STAMINA_FRAMES];                      // Масив текстур для кожного рівня стаміни
};