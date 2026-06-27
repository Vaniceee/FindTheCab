#include "UI.h"
#include "../core/AssetManager.h"

StaminaBar::StaminaBar() {												// Конструктор: занулюємо всі елементи масиву для безпеки

	for (int i = 0; i < STAMINA_FRAMES; i++) {
		staminaTextures[i] = { 0 };
	}

}
void StaminaBar::Initialize() {												// Завантаження ресурсів по посиланню з менеджера
	AssetManager& am = AssetManager::Instance();

	staminaTextures[0] = am.GetTexture("stamina_0");		// 0%
	staminaTextures[1] = am.GetTexture("stamina_1");		// 10%
	staminaTextures[2] = am.GetTexture("stamina_2");		// 20%
	staminaTextures[3] = am.GetTexture("stamina_3");		// 30%
	staminaTextures[4] = am.GetTexture("stamina_4");		// 40%
	staminaTextures[5] = am.GetTexture("stamina_5");		// 50%
	staminaTextures[6] = am.GetTexture("stamina_6");		// 60%
	staminaTextures[7] = am.GetTexture("stamina_7");		// 70%
	staminaTextures[8] = am.GetTexture("stamina_8");		// 80%
	staminaTextures[9] = am.GetTexture("stamina_9");		// 90%
	staminaTextures[10] = am.GetTexture("stamina_10");		// 100%

}

void StaminaBar::Draw(float currentStamina, Vector2 position) {			// Рендеринг шкали стаміни

int frameIndex = (int)(currentStamina / 10.0f);							// Перетворюємо стаміну (0-100) в індекс масиву (0-10)

if (frameIndex < 0) frameIndex = 0;										// Страховка від виходу за межі масиву

if (frameIndex > 10) frameIndex = 10;

float scale = 4.0f;

DrawTextureEx(staminaTextures[frameIndex], position, 0.0f, scale, WHITE);

}