/**
* MIT License
*
* Copyright (c) 2024 Tomas Vecera, tomas@vecera.dev
*
* The UI of this software is based on the Volos ESP32 Li-ion Battery Charger:
*   Repository: https://github.com/VolosR/chargerM5StickC
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
* documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
* Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "flashlight.h"

FlashLight::FlashLight(LGFX_Sprite *spr) {
	sprite = spr;
}

void FlashLight::drawSensorData(uint16_t background) {
	sprite->fillCircle(DISPLAY_WIDTH / 2, (DISPLAY_HEIGHT / 2) - 20, 40, turnOff ? TFT_RED : TFT_DARKGREEN);
	sprite->setTextDatum(MC_DATUM);
	sprite->setTextColor(TFT_WHITE, turnOff ? TFT_RED : TFT_DARKGREEN);
	sprite->setTextFont(4);
	sprite->drawString(turnOff ? F("OFF") : F("ON"), DISPLAY_WIDTH / 2, (DISPLAY_HEIGHT / 2) - 15);
}

#ifndef EMULATOR
[[noreturn]] void FlashLight::timer(void *pvParameters) {
	auto *instance = static_cast<FlashLight *>(pvParameters);
	bool enabled = false;

	while (true) {
		if (xSemaphoreTake(instance->mutex, portTICK_PERIOD_MS * 10) == pdTRUE) {
			enabled = instance->enabled;
			xSemaphoreGive(instance->mutex);
		}
		if (enabled) {
			digitalWrite(PORT_A_YELLOW_PIN, HIGH);
			delay(100);
			digitalWrite(PORT_A_YELLOW_PIN, LOW);
			delay(100);
			digitalWrite(PORT_A_YELLOW_PIN, HIGH);
			delay(100);
			digitalWrite(PORT_A_YELLOW_PIN, LOW);
			delay(500);
		} else {
			digitalWrite(PORT_A_YELLOW_PIN, LOW);
		}
	}
}
#endif

void FlashLight::open() {
	bool connected = splash(F("Flash Light"), F("Run"), FlashLightUnitImg, 100, 100, false);

	if (connected) {
		unit_connected();
#ifndef EMULATOR
		pinMode(PORT_A_YELLOW_PIN, OUTPUT);
		digitalWrite(PORT_A_YELLOW_PIN, LOW);
		delay(50);

		mutex = xSemaphoreCreateMutex();
#ifdef M5STICK
		xTaskCreatePinnedToCore(timer, "flashlight", 4096, this, 2 | portPRIVILEGE_BIT, &task, 0);
#endif
#ifdef PICOPAD
		xTaskCreate(timer, "flashlight", 4096, this, 2 | portPRIVILEGE_BIT, &task);
#endif
#endif
		while (!M5.BtnPWR.wasClicked()) {
			default_actions();

			if (M5.BtnA.wasClicked()) {
#ifndef EMULATOR
				if (xSemaphoreTake(mutex, portTICK_PERIOD_MS * 10) == pdTRUE) {
					enabled = !enabled;
					turnOff = !turnOff;
					xSemaphoreGive(mutex);
				}
#else
  			enabled = !enabled;
	  		turnOff = !turnOff;
#endif
			}
			drawAll(F("Flash On/Off"));
			M5.update();
		}
	}
#ifndef EMULATOR
	if (task != nullptr) { vTaskDelete(task); }
	if (mutex != nullptr) { vSemaphoreDelete(mutex); }
#endif
}
