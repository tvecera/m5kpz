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
#include "tof.h"

Tof::Tof(LGFX_Sprite *spr) {
	sprite = spr;
}

bool Tof::initSensor() {
#ifdef EMULATOR
	return true;
#else
#ifdef M5STICK
	return (Wire.begin(0, 26, 100000UL) && sensor.init(true));
#else
	Wire.setSDA(PORT_A_SDA_PIN);
	Wire.setSCL(PORT_A_SCL_PIN);
	Wire.setTimeout(500);
	Wire.begin();
	return sensor.init(true);
#endif
#endif
}

void Tof::drawSensorData(uint16_t background) {
	sprite->setTextDatum(MC_DATUM);
	sprite->setTextColor(TFT_WHITE, background);
	sprite->setFont(&DSEG7_Classic_Bold_32);
	sprite->drawString(String(distance).c_str(), DISPLAY_WIDTH / 2, 70);

	sprite->setTextDatum(MC_DATUM);
	sprite->setFont(&DSEG7_Classic_Bold_17);
	sprite->drawString(F("mm"), DISPLAY_WIDTH / 2, 120);
}

void Tof::open() {
#ifndef EMULATOR
	sensor.setTimeout(500);
#endif
	bool connected = splash(F("ToF HAT"), F("Connect"), TofUnitImg, 100, 90, false);

	if (connected) {
		unit_connected();

#ifndef EMULATOR
		sensor.setSignalRateLimit(0.1);
		sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
		sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
#endif
		while (!M5.BtnPWR.wasClicked()) {
			default_actions();

#ifndef EMULATOR
			distance = sensor.readRangeSingleMillimeters();
			if (sensor.timeoutOccurred() || distance >= 8190) {
				distance = 0;
			}
#else
			distance = 256;
#endif
			drawAll(F("Distance"));
			delay(100);
			M5.update();
		}
	}
#ifndef EMULATOR
	Wire.end();
#endif
}
