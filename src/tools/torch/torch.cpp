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
#include "torch.h"

Torch::Torch(LGFX_Sprite *spr) {
	sprite = spr;
}

void Torch::drawSensorData(uint16_t background) {
	sprite->fillCircle(DISPLAY_WIDTH / 2, (DISPLAY_HEIGHT / 2) - 20, 40, enabled ? TFT_RED: TFT_DARKGREEN);
	sprite->setTextDatum(MC_DATUM);
	sprite->setTextColor(TFT_WHITE, enabled ? TFT_RED: TFT_DARKGREEN);
	sprite->setTextFont(4);
	sprite->drawString(enabled ? F("OFF") : F("ON"), DISPLAY_WIDTH / 2, (DISPLAY_HEIGHT / 2) - 15);
}

void Torch::open() {
	bool connected = splash(F("Torch"), F("Run"), LightUnitImg, 64, 100, false);

	if (connected) {
		unit_connected();
#ifndef EMULATOR
		pinMode(PORT_A_YELLOW_PIN, OUTPUT);
		digitalWrite(PORT_A_YELLOW_PIN, LOW);
#endif
		delay(50);

		while (!M5.BtnPWR.wasClicked()) {
			default_actions();

			if (M5.BtnA.wasClicked()) {
				enabled = !enabled;
#ifndef EMULATOR
				digitalWrite(PORT_A_YELLOW_PIN, enabled ? HIGH : LOW);
#endif
			}

			drawAll(F("Torch On/Off"));
			M5.update();
		}
	}
#ifndef EMULATOR
	digitalWrite(PORT_A_YELLOW_PIN, LOW);
#endif
}
