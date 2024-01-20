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
#include "tvoc.h"

Tvoc::Tvoc(LGFX_Sprite *spr) {
	sprite = spr;
}

bool Tvoc::initSensor() {
#ifdef EMULATOR
	return true;
#else
#ifdef M5STICK
	return (Wire.begin(PORT_A_SDA_PIN, PORT_A_SCL_PIN) && sgp.begin());
#else
	Wire.setSCL(PORT_A_SCL_PIN);
	Wire.setSDA(PORT_A_SDA_PIN);
	Wire.begin();
	return sgp.begin();
#endif
#endif
}

void Tvoc::drawSensorData(uint16_t background) {
	sprite->setTextDatum(MC_DATUM);
	sprite->setTextColor(TFT_WHITE, background);
	sprite->setFont(&DSEG7_Classic_Bold_32);
	sprite->drawString(String(eCO2).c_str(), DISPLAY_WIDTH / 2, 70);

	sprite->fillRoundRect(10, 112, DISPLAY_WIDTH - 20, 46, 4, COLOR);

	sprite->setTextDatum(MC_DATUM);
	sprite->setTextColor(TFT_WHITE, COLOR);
	sprite->setTextFont(2);
	sprite->drawString(F("TVOC ppb"), DISPLAY_WIDTH / 2, 122);
	sprite->setFont(&DSEG7_Classic_Bold_17);
	sprite->drawString(String(TVOC).c_str(), DISPLAY_WIDTH / 2, 142);
}

void Tvoc::open() {
	bool connected = splash(F("TVOC/eCO2 Unit"), F("Connect"), TVOCUnitImg, 100, 99, true);

	if (connected) {
		unit_connected();

		int i = 15;
		sprite->setTextDatum(MC_DATUM);
		sprite->setTextColor(TFT_WHITE, TFT_BLACK);
		sprite->setTextFont(2);
		sprite->drawString(F("Warming..."), DISPLAY_WIDTH / 2, 80);
		while (i > 0) {
			if (millis() - last_millis > 1000) {
				last_millis = millis();
				i--;
				sprite->fillRect(10, 100, 120, 60, TFT_BLACK);
				sprite->setFont(&DSEG7_Classic_Bold_32);
				sprite->drawNumber(i, DISPLAY_WIDTH / 2, 130);
				sprite->pushSprite(START_X, START_Y);
			}
		}

		sprite->fillSprite(TFT_BLACK);
		sprite->pushSprite(START_X, START_Y);

		while (!M5.BtnPWR.wasClicked()) {
			default_actions();

#ifndef EMULATOR
			if (sgp.IAQmeasure()) {
				TVOC = sgp.TVOC;
				eCO2 = sgp.eCO2;
			} else {
				TVOC = 0;
				eCO2 = 0;
			}
#else
			TVOC = 2;
  		eCO2 = 400;
#endif
			drawAll(F("eCO2 ppm"));
			delay(100);
			M5.update();
		}
	}
#ifdef M5STICK
	sgp.end();
#endif
}
