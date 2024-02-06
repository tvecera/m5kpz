/**
* MIT License
*
* Copyright (c) 2024 Ondrej Kolonicny, ondra@ok1cdj.com
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
#include "pwrmeter.h"


Pwrmeter::Pwrmeter(LGFX_Sprite *spr) {
	sprite = spr;
}

bool Pwrmeter::initSensor() {
#ifdef EMULATOR
	return true;
#else
#ifdef M5STICK
	return (Wire.begin(0, 26) && ina219.begin());
#else
	Wire.setSCL(PORT_A_SCL_PIN);
	Wire.setSDA(PORT_A_SDA_PIN);
	Wire.begin();
	return (ina219.begin());
#endif
#endif
}

void Pwrmeter::drawSensorData(uint16_t background) {
	sprite->setTextColor(TFT_WHITE, TFT_BLACK);
	sprite->setFont(&DSEG7_Classic_Bold_32);
	sprite->drawString(String(loadvoltage, 2).c_str(), 10, 30);

	sprite->fillRoundRect(10, 72, DISPLAY_WIDTH - 20, 46, 4, COLOR);
	sprite->fillRoundRect(10, 123, DISPLAY_WIDTH - 20, 46, 4, COLOR);

	sprite->setTextDatum(MC_DATUM);
	sprite->setTextColor(TFT_WHITE, COLOR);
	sprite->setTextFont(2);
	sprite->drawString(F("Current mA"), DISPLAY_WIDTH / 2, 82);
	sprite->drawString(F("Power W"), DISPLAY_WIDTH / 2, 133);
	sprite->setFont(&DSEG7_Classic_Bold_17);
	sprite->drawString(String(current_mA, 2).c_str(), DISPLAY_WIDTH / 2, 102);
	sprite->drawString(String(busvoltage*current_mA/1000, 3).c_str(), DISPLAY_WIDTH / 2, 153);

}

#ifndef EMULATOR

void Pwrmeter::timer(void *pvParameters) {
	auto *instance = static_cast<Pwrmeter *>(pvParameters);

	float timer_shuntvoltage = 0;
  	float timer_busvoltage = 0;
  	float timer_current_mA = 0;
  	float timer_loadvoltage = 0;

	while (true) {
		
		if (xSemaphoreTake(instance->mutex, portTICK_PERIOD_MS * 100) == pdTRUE) {
					
		timer_shuntvoltage = instance->ina219.getShuntVoltage_mV();
  		timer_busvoltage = instance->ina219.getBusVoltage_V();
  		timer_current_mA = instance->ina219.getCurrent_mA();
  		timer_loadvoltage = timer_busvoltage + (timer_shuntvoltage / 1000);

			instance->shuntvoltage = timer_shuntvoltage;
			instance->busvoltage = timer_busvoltage;
			instance->current_mA = timer_current_mA;
			instance->loadvoltage = timer_loadvoltage;
			xSemaphoreGive(instance->mutex);
		}
	}
}

#endif

void Pwrmeter::open() {
	bool connected = splash(F("PWR meter"), F("Connect"), PwrmeterImg, 100, 100, true);

	if (connected) {
		unit_connected();

#ifndef EMULATOR
		mutex = xSemaphoreCreateMutex();
#ifdef M5STICK
		xTaskCreatePinnedToCore(timer, "pwrmeter", 4096, this, 2 | portPRIVILEGE_BIT, &task, 0);
#else
		xTaskCreate(timer, "pwrmeter", 4096, this, 2 | portPRIVILEGE_BIT, &task);
#endif
#endif

		while (!M5.BtnPWR.wasClicked()) {
			default_actions();

#ifndef EMULATOR
			if (xSemaphoreTake(mutex, portTICK_PERIOD_MS * 100) == pdTRUE) {
				shuntvoltage = sensor_shuntvoltage;
				busvoltage = sensor_busvoltage;
				current_mA = sensor_current_mA;
				loadvoltage = sensor_loadvoltage;

				xSemaphoreGive(mutex);
			}
#else
			shuntvoltage = 0.100;
			busvoltage = 5.00;
			current_mA = 1000.0;
			loadvoltage = busvoltage + (shuntvoltage / 1000);
#endif
			drawAll(F("Voltage V"));
			M5.update();
		}
	}
#ifndef EMULATOR
	if (task != nullptr) { vTaskDelete(task); }
	if (mutex != nullptr) { vSemaphoreDelete(mutex); }
	Wire.end();
#endif
}
