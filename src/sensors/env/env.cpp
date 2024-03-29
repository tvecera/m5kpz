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
#include "env.h"

#define ENV_BMP280_ADDRESS 0x76

Env::Env(LGFX_Sprite *spr) {
	sprite = spr;
}

bool Env::initSensor() {
#ifdef EMULATOR
	return true;
#else
#ifdef M5STICK
	return (Wire.begin(0, 26) && bme280.begin(ENV_BMP280_ADDRESS));
#else
	Wire.setSCL(PORT_A_SCL_PIN);
	Wire.setSDA(PORT_A_SDA_PIN);
	Wire.begin();
	return (bme280.begin(ENV_BMP280_ADDRESS));
#endif
#endif
}

void Env::drawSensorData(uint16_t background) {
	sprite->setTextColor(TFT_WHITE, background);
	sprite->setFont(&DSEG7_Classic_Bold_32);
	sprite->drawString(String(tmp).c_str(), 10, 30);

	sprite->fillRoundRect(10, 72, DISPLAY_WIDTH - 20, 46, 4, COLOR);
	sprite->fillRoundRect(10, 123, DISPLAY_WIDTH - 20, 46, 4, COLOR);

	sprite->setTextDatum(MC_DATUM);
	sprite->setTextColor(TFT_WHITE, COLOR);
	sprite->setTextFont(2);
	sprite->drawString(F("HUMIDITY %"), DISPLAY_WIDTH / 2, 82);
	sprite->drawString(F("PRESSURE hPa"), DISPLAY_WIDTH / 2, 133);
	sprite->setFont(&DSEG7_Classic_Bold_17);
	sprite->drawString(String(hum).c_str(), DISPLAY_WIDTH / 2, 102);
	sprite->drawString(String(pressure, 2).c_str(), DISPLAY_WIDTH / 2, 153);
}

#ifndef EMULATOR

void Env::timer(void *pvParameters) {
	auto *instance = static_cast<Env *>(pvParameters);

	float timer_tmp;
	float timer_hum;
	float timer_pressure;

	while (true) {
		timer_pressure = instance->bme280.readPressure();
		timer_pressure = timer_pressure / 100;
		timer_tmp = instance->dht12.readTemperature();
		timer_hum = instance->dht12.readHumidity();
	
		if (xSemaphoreTake(instance->mutex, portTICK_PERIOD_MS * 10) == pdTRUE) {
			instance->sensor_tmp = timer_tmp;
			instance->sensor_hum = timer_hum;
			instance->sensor_pressure = timer_pressure;
			xSemaphoreGive(instance->mutex);
		}
	}
}

#endif

void Env::open() {
	bool connected = splash(F("ENV HAT"), F("Connect"), EnvUnitImg, 100, 99, true);

	if (connected) {
		unit_connected();

#ifndef EMULATOR
		mutex = xSemaphoreCreateMutex();
#ifdef M5STICK
		xTaskCreatePinnedToCore(timer, "env", 4096, this, 2 | portPRIVILEGE_BIT, &task, 0);
#else
		xTaskCreate(timer, "enviii", 4096, this, 2 | portPRIVILEGE_BIT, &task);
#endif
#endif

		while (!M5.BtnPWR.wasClicked()) {
			default_actions();

#ifndef EMULATOR
			if (xSemaphoreTake(mutex, portTICK_PERIOD_MS * 10) == pdTRUE) {
				tmp = sensor_tmp;
				hum = sensor_hum;
				pressure = sensor_pressure;
				xSemaphoreGive(mutex);
			}
#else
			tmp = 24.2;
			hum = 61.2;
			pressure = 982.25;
#endif
			drawAll(F("TEMP C"));
			M5.update();
		}
	}
#ifndef EMULATOR
	if (task != nullptr) { vTaskDelete(task); }
	if (mutex != nullptr) { vSemaphoreDelete(mutex); }
	Wire.end();
#endif
}
