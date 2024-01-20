/**
* MIT License
*
* Copyright (c) 2024 Tomas Vecera, tomas@vecera.dev
*
* This software is based on the Volos ESP32 Li-ion Battery Charger and M5StickC-Plus Dices example:
*   Repository:
 *   - https://github.com/VolosR/chargerM5StickC
 *   - https://github.com/m5stack/M5StickC-Plus/tree/master/examples/Games/Dices
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
#include "dices.h"

#define DOT_SIZE 6

Dices::Dices(LGFX_Sprite *spr) {
	sprite = spr;
}

void Dices::draw_dice(int16_t x, int16_t y, uint32_t n) {
	sprite->fillRect(x, y, 70, 70, TFT_WHITE);

	for (int d = 0; d < 6; d++) {
		if (dot[n][d][0] > 0) {
			sprite->fillCircle(x + dot[n][d][0], y + dot[n][d][1], DOT_SIZE, TFT_BLACK);
		}
	}
}

void Dices::drawSensorData(uint16_t background) {
	if (!two_dices && number1 >= 0) {
		draw_dice((135 / 2) - 35, 80, number1);
	}
	if (two_dices && number1 >= 0 && number2 >= 0) {
		draw_dice((135 / 2) - 35, 45, number1);
		draw_dice((135 / 2) - 35, 125, number2);
	}
}

void Dices::open() {

	bool connected = splash(F("Dices"), F("Run"), DicesUnitImg, 100, 96, false);

	if (connected) {
		unit_connected();

#ifdef M5STICK
		M5.Imu.begin();
		useRealRandomGenerator(true);
#endif

		while (!M5.BtnPWR.wasClicked()) {
			M5.update();
			default_actions();

#ifdef M5STICK
			M5.Imu.getAccelData(&accX, &accY, &accZ);
#endif
#ifdef EMULATOR
			if (M5.BtnC.wasClicked()) { accX = 2; }
			else {
				accX = 0;
				accY = 0;
			}
#endif
#ifdef PICOPAD
			if (M5.BtnLeft.wasClicked() || M5.BtnRight.wasClicked() || M5.BtnUp.wasClicked() ||
					M5.BtnDown.wasClicked()) { accX = 2; }
			else {
				accX = 0;
				accY = 0;
			}
#endif
			if (accX > 1.5 || accY > 1.5) {
				M5.update();
				number1 = RANDOM(0, 6);
				delay(500);
				if (two_dices) {
					number2 = RANDOM(0, 6);
					delay(500);
				}
			}

			if (M5.BtnA.wasClicked()) {
				two_dices = !two_dices;
				number1 = 0;
				number2 = 0;
			}

			drawAll(F("Shake Me"), TFT_BLACK, true, true);
		}
	}
}
