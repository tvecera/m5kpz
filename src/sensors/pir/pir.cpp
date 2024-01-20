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
#include "pir.h"
#ifdef PICOPAD
#include "sound/alarm.c"
#endif

#ifdef M5STICK
#define SERVO_PIN 26
#define LED_CHANNEL 0
#endif

#ifndef EMULATOR
[[noreturn]] void Pir::alarm(void *pvParameters) {
	auto *instance = static_cast<Pir *>(pvParameters);
	bool currentMotion;

	while (true) {
		if (xSemaphoreTake(instance->mutex, portTICK_PERIOD_MS * 100) == pdTRUE) {
			 currentMotion = instance->motion;
			xSemaphoreGive(instance->mutex);
			if (currentMotion) {
				digitalWrite(M5_LED, LOW);
#ifdef M5STICK
				ledcWriteTone(LED_CHANNEL, 1800);
#endif
				delay(1000);
				digitalWrite(M5_LED, HIGH);
#ifdef M5STICK
				ledcWriteTone(LED_CHANNEL, 0);
#endif
				delay(200);
			} else {
				digitalWrite(M5_LED, HIGH);
#ifdef M5STICK
				ledcWriteTone(LED_CHANNEL, 0);
#endif
				delay(200);
			}
		}
	}
}
#endif

Pir::Pir(LGFX_Sprite *spr) {
	sprite = spr;
}

void Pir::drawSensorData(uint16_t background) {
	if (background != TFT_BLACK) {
		sprite->setTextDatum(MC_DATUM);
		sprite->setTextColor(TFT_BLACK, background);
		sprite->setFont(&DSEG7_Classic_Bold_32);
		sprite->drawString(F("1"), DISPLAY_WIDTH / 2, 90);
	} else {
		sprite->setTextDatum(MC_DATUM);
		sprite->setTextColor(TFT_WHITE, background);
		sprite->setFont(&DSEG7_Classic_Bold_32);
		sprite->drawString(F("0"), DISPLAY_WIDTH / 2, 90);
	}
}

void Pir::open() {
	bool connected = splash(F("Motion Alarm"), F("Run"), PirUnitImg, 100, 132, false);

	if (connected) {
		unit_connected();

#ifndef EMULATOR
		pinMode(PORT_A_WHITE_PIN, INPUT);
		pinMode(M5_LED, OUTPUT);
		digitalWrite(M5_LED, HIGH);
		mutex = xSemaphoreCreateMutex();
#ifdef M5STICK
		ledcSetup(LED_CHANNEL, freq, resolution);
		ledcAttachPin(SERVO_PIN, LED_CHANNEL);
		ledcWrite(LED_CHANNEL, 256);
		ledcWriteTone(LED_CHANNEL, 0);

		xTaskCreatePinnedToCore(alarm, "pir_alarm", 4096, this, 2 | portPRIVILEGE_BIT, &task, 0);
#else
		xTaskCreate(alarm, "pir_alarm", 4096, this, 2 | portPRIVILEGE_BIT, &task);
#endif
#endif

		bool currentMotion;

		while (!M5.BtnPWR.wasClicked()) {
#ifndef EMULATOR
			xSemaphoreTake(mutex, portMAX_DELAY);
			bool pirValue = digitalRead(PORT_A_WHITE_PIN) == 1;
			if (pirValue && !motion) {
				M5.Display.setBrightness(brightness[4]);
#ifdef PICOPAD
				PLAYSOUNDREP(AlarmSnd);
#endif
			}
			motion = motion ? motion : pirValue;
			currentMotion = motion;
			xSemaphoreGive(mutex);
#endif

			default_actions();

			if (M5.BtnA.wasClicked()) {
#ifndef EMULATOR
				M5.Display.setBrightness(brightness[chosenB]);
				xSemaphoreTake(mutex, portMAX_DELAY);
				motion = false;
#ifdef PICOPAD
				StopSound();
#endif
				xSemaphoreGive(mutex);
#else
				currentMotion = !currentMotion;
#endif
			}

			drawAll(F("Motion"), currentMotion ? TFT_RED : TFT_BLACK);
			M5.update();
		}

#ifndef EMULATOR
		digitalWrite(M5_LED, 0);
		M5.Display.setBrightness(brightness[chosenB]);
#ifdef M5STICK
		ledcWriteTone(LED_CHANNEL, 0);
		ledcDetachPin(SERVO_PIN);
#endif
#endif
	}
#ifndef EMULATOR
	if (task != nullptr) { vTaskDelete(task); }
	if (mutex != nullptr) { vSemaphoreDelete(mutex); }
#endif
}
