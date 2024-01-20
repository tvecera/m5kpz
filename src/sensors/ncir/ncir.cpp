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
#include "ncir.h"

Ncir::Ncir(LGFX_Sprite *spr) {
	sprite = spr;
}

bool Ncir::initSensor() {
#ifndef EMULATOR
	return ncir2.begin(&Wire, PORT_A_YELLOW_PIN, PORT_A_WHITE_PIN, M5UNIT_NCIR2_DEFAULT_ADDR);
#else
	return true;
#endif
}

void Ncir::drawSensorData(uint16_t background) {
	sprite->setTextColor(TFT_WHITE, background);
	sprite->setFont(&DSEG7_Classic_Bold_32);
	sprite->drawString(String(temperature, 2).c_str(), 10, 30);

	sprite->fillRoundRect(10, 72, DISPLAY_WIDTH - 20, 46, 4, COLOR);
	sprite->fillRoundRect(10, 123, DISPLAY_WIDTH - 20, 46, 4, COLOR);

	sprite->setTextDatum(MC_DATUM);
	sprite->setTextColor(TFT_WHITE, COLOR);
	sprite->setTextFont(2);
	sprite->drawString(F("TEMP SOC C"), DISPLAY_WIDTH / 2, 82);
	sprite->drawString(F("EMISSIVITY"), DISPLAY_WIDTH / 2, 133);
	sprite->setFont(&DSEG7_Classic_Bold_17);
	sprite->drawString(String(temperature_soc, 2).c_str(), DISPLAY_WIDTH / 2, 102);
	sprite->drawString(String(emissivity, 2).c_str(), DISPLAY_WIDTH / 2, 153);
}

void Ncir::open() {

	bool connected = splash(F("NCIR 2 Unit"), F("Connect"), NcirUnitImg, 67, 131, true);

	if (connected) {
		unit_connected();

		while (!M5.BtnPWR.wasClicked()) {
			default_actions();

#ifndef EMULATOR
			if (ncir2.getVersion()) {
				temperature = (float) ncir2.getTempValue() / 100;
				temperature_soc = (float) ncir2.getSocTempValue() / 100;
				emissivity = ncir2.getEmissivity();
			} else {
				temperature = 0.0;
				temperature_soc = 0.0;
				emissivity = 0.0;
			}
#else
			temperature = 21.81;
			temperature_soc = 24.22;
			emissivity = 0.95;
#endif
			drawAll(F("TEMP C"));
			M5.update();
		}
	}
#ifndef EMULATOR
	Wire.end();
#endif
}