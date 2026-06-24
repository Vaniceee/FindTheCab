#pragma once
#include "raylib.h"

class StaminaBar {
private:

static const int STAMINA_FRAMES = 11;

Texture2D staminaTextures[STAMINA_FRAMES]; public:

	// Конструктор класу (викликається автоматично при створенні об'єкта)

	StaminaBar();

	// Метод для завантаження картинки у пам'ять

	void Load();

	// Метод для малювання шкали на екрані

	void Draw(float currentStamina, Vector2 position);

	// Метод для вивантаження текстур (очищення пам'яті)

	void Unload();

};