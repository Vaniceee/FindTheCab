#include "UI.h"

StaminaBar::StaminaBar() {													// Конструктор: занулюємо всі елементи масиву для безпеки

	for (int i = 0; i < STAMINA_FRAMES; i++) {
		staminaTextures[i] = { 0 };
	}

}
void StaminaBar::Load() {													// Завантаження ресурсів

	staminaTextures[0] = LoadTexture("assets/meters/empty0.png");		// 0%
	staminaTextures[1] = LoadTexture("assets/meters/red1.png");		// 10%
	staminaTextures[2] = LoadTexture("assets/meters/red2.png");		// 20%
	staminaTextures[3] = LoadTexture("assets/meters/red3.png");		// 30%
	staminaTextures[4] = LoadTexture("assets/meters/orange4.png");	// 40%
	staminaTextures[5] = LoadTexture("assets/meters/orange5.png");	// 50%
	staminaTextures[6] = LoadTexture("assets/meters/orange6.png");	// 60%
	staminaTextures[7] = LoadTexture("assets/meters/orange7.png");	// 70%
	staminaTextures[8] = LoadTexture("assets/meters/green8.png");		// 80%
	staminaTextures[9] = LoadTexture("assets/meters/green9.png");		// 90%
	staminaTextures[10] = LoadTexture("assets/meters/green10.png");	// 100%

}

void StaminaBar::Draw(float currentStamina, Vector2 position) {				// Рендеринг шкали стаміни

int frameIndex = (int)(currentStamina / 10.0f);								// Перетворюємо стаміну (0-100) в індекс масиву (0-10)

if (frameIndex < 0) frameIndex = 0;											// Страховка від виходу за межі масиву

if (frameIndex > 10) frameIndex = 10;

float scale = 4.0f;

DrawTextureEx(staminaTextures[frameIndex], position, 0.0f, scale, WHITE);

}

void StaminaBar::Unload() {													// Очищення пам'яті
for (int i = 0; i < STAMINA_FRAMES; i++) {
	if (staminaTextures[i].id > 0) {										// Перевіряємо, чи текстура дійсно була завантажена, перед тим як її видаляти
		UnloadTexture(staminaTextures[i]);
	}
}
}