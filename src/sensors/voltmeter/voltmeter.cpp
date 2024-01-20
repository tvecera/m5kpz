/**
* MIT License
*
* Copyright (c) 2024 Tomas Vecera, tomas@vecera.dev
*
* This software is based on the Volos Voltmeter:
*   Repository: https://github.com/VolosR/M5stackVoltmeter
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
#include "voltmeter.h"

VoltmeterModule::VoltmeterModule(LGFX_Sprite *spr) {
	sprite = spr;
}

void VoltmeterModule::drawSensorData(uint16_t background) {
	sprite->setTextColor(TFT_WHITE, TFT_BLACK);
	sprite->setFont(&DSEG7_Classic_Bold_32);
	sprite->drawString(String(voltage, (d ? 3 : 2)).c_str(), 10, 30);

	sprite->fillRoundRect(10, 72, DISPLAY_WIDTH - 20, 46, 4, COLOR);
	sprite->fillRoundRect(10, 123, DISPLAY_WIDTH - 20, 46, 4, COLOR);

	sprite->setTextDatum(MC_DATUM);
	sprite->setTextColor(TFT_WHITE, COLOR);
	sprite->setTextFont(2);
	sprite->drawString(F("ADC RAW"), DISPLAY_WIDTH / 2, 82);
	sprite->drawString(F("Voltage RAW"), DISPLAY_WIDTH / 2, 133);
	sprite->setFont(&DSEG7_Classic_Bold_17);
	sprite->drawString(String(adc_raw).c_str(), DISPLAY_WIDTH / 2, 102);
	sprite->drawString(String(voltage_raw, 2).c_str(), DISPLAY_WIDTH / 2, 153);
}

void VoltmeterModule::open() {
	bool connected = splash(F("Voltmeter Unit"), F("Connect"), VoltmeterUnitImg, 56, 131, false);

	if (connected) {
		unit_connected();
#ifndef EMULATOR
#ifdef M5STICK
		Wire.begin(PORT_A_SDA_PIN, PORT_A_SCL_PIN);
#endif
#ifdef PICOPAD
		Wire.setSCL(PORT_A_SCL_PIN);
		Wire.setSDA(PORT_A_SDA_PIN);
		Wire.begin();
#endif
		voltmeter.setMode(SINGLESHOT);
		voltmeter.setRate(RATE_128);
		voltmeter.setGain(PAG_512);
#endif

		while (!M5.BtnPWR.wasClicked()) {
			default_actions();

			if (M5.BtnA.wasClicked()) {
				d = !d;
			}

#ifndef EMULATOR
			voltmeter.getVoltage();
			volt_raw_list[raw_now_ptr] = voltmeter.adc_raw;
			raw_now_ptr = (raw_now_ptr == 9) ? 0 : (raw_now_ptr + 1);

			int count = 0;
			int total = 0;

			for (short i: volt_raw_list) {
				if (i == 0) {
					continue;
				}
				total += i;
				count += 1;
			}

			if (count == 0) {
				adc_raw = 0;
			} else {
				adc_raw = total / count;
			}

			voltage_raw = adc_raw * voltmeter.resolution * voltmeter.calibration_factor;
			voltage = voltage_raw / 1000.0;
#else
			adc_raw = 1024;
			voltage_raw = 1258.7;
			voltage = 1.2587;
#endif
			drawAll(F("Voltage V"));
			delay(100);
			M5.update();
		}
	}
#ifndef EMULATOR
	Wire.end();
#endif
}
