/**
* MIT License
*
* Copyright (c) 2023 Tomas Vecera, tomas@vecera.dev
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
#include "joystick.h"
#include "Wire.h"

#define JOY_ADDR 0x38
#define MUTEX_TIMEOUT 10
#define KEY_TIMEOUT 5

Joystick::Joystick() {
	xj = 0;
	yj = 0;
	bj = 0;
}

bool Joystick::begin() {
	bool result = false;
	if (Wire.begin(0, 26, 100000UL)) {
		if (mutex == nullptr){
			mutex = xSemaphoreCreateMutex();
		}
		result = check();
		if (result) {
			xTaskCreatePinnedToCore(timer, "joy", 4096, this, 2 | portPRIVILEGE_BIT, &task, 0);
		}
	}

	return result;
}

void Joystick::end() {
	if (task != nullptr) { vTaskDelete(task); }
	if (mutex != nullptr) { vSemaphoreDelete(mutex); }
	Wire.end();
}

bool Joystick::check() {
	bool result = false;
	if (xSemaphoreTake(mutex, portTICK_PERIOD_MS * MUTEX_TIMEOUT) == pdTRUE) {
		Wire.beginTransmission(JOY_ADDR);
		Wire.write(0x02);
		Wire.endTransmission();
		Wire.requestFrom(JOY_ADDR, 3);
		if (Wire.available()) {
			xj = int8_t(Wire.read());
			yj = int8_t(Wire.read());
			bj = int8_t(Wire.read());
			result = true;
		}
		xSemaphoreGive(mutex);
	}

	return result;
}

bool Joystick::isLeft() {
	bool result = false;
	if (xSemaphoreTake(mutex, portTICK_PERIOD_MS * MUTEX_TIMEOUT) == pdTRUE) {
		result = xj > 100;
		xSemaphoreGive(mutex);
	}
	return result;
}

bool Joystick::isRight() {
	bool result = false;
	if (xSemaphoreTake(mutex, portTICK_PERIOD_MS * MUTEX_TIMEOUT) == pdTRUE) {
		result = xj < -100;
		xSemaphoreGive(mutex);
	}
	return result;
}

bool Joystick::isClick() {
	bool result = false;
	if (xSemaphoreTake(mutex, portTICK_PERIOD_MS * MUTEX_TIMEOUT) == pdTRUE) {
		result = bj == 0;
		xSemaphoreGive(mutex);
	}
	return result;
}

bool Joystick::isUp() {
	bool result = false;
	if (xSemaphoreTake(mutex, portTICK_PERIOD_MS * MUTEX_TIMEOUT) == pdTRUE) {
		result = yj < -100;
		xSemaphoreGive(mutex);
	}
	return result;
}

bool Joystick::isDown() {
	bool result = false;
	if (xSemaphoreTake(mutex, portTICK_PERIOD_MS * MUTEX_TIMEOUT) == pdTRUE) {
		result = yj > 100;
		xSemaphoreGive(mutex);
	}
	return result;
}

int8_t Joystick::getX() {
	int8_t result = 0;
	if (xSemaphoreTake(mutex, portTICK_PERIOD_MS * MUTEX_TIMEOUT) == pdTRUE) {
		result = xj;
		xSemaphoreGive(mutex);
	}
	return result;
}

int8_t Joystick::getY() {
	int8_t result = 0;
	if (xSemaphoreTake(mutex, portTICK_PERIOD_MS * MUTEX_TIMEOUT) == pdTRUE) {
		result = yj;
		xSemaphoreGive(mutex);
	}
	return result;
}

int8_t Joystick::getB() {
	int8_t result = 1;
	if (xSemaphoreTake(mutex, portTICK_PERIOD_MS * MUTEX_TIMEOUT) == pdTRUE) {
		result = bj;
		xSemaphoreGive(mutex);
	}
	return result;
}

void Joystick::timer(void *pvParameters) {
	auto* instance = static_cast<Joystick*>(pvParameters);

	while (true) {
		instance->check();
		delay(KEY_TIMEOUT);
	}
}