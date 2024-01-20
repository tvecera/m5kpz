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
#include "M5.h"

M5Button_Class::M5Button_Class(int typeValue) {
	type = typeValue;
}

bool M5Button_Class::wasClicked() { return type == key; }


void M5Display_Class::setBrightness(uint8_t brightness) {
	uint PWM_GPIO_SLICE = pwm_gpio_to_slice_num(DISP_BLK_PIN);
	uint PWM_GPIO_CHANNEL = pwm_gpio_to_channel(DISP_BLK_PIN);
	pwm_set_chan_level(PWM_GPIO_SLICE, PWM_GPIO_CHANNEL, brightness);
}

int M5Power_Class::getBatteryVoltage() {
	BatInit();
	int result = GetBatInt();
	BatTerm();
	return result;
}

M5Common::M5Common() = default;

void M5Common::update() {
	KeyScan();
	int ch = KeyGet();
	BtnA.key = ch;
	BtnB.key = ch;
	BtnPWR.key = ch;
	BtnY.key = ch;
	BtnLeft.key = ch;
	BtnRight.key = ch;
	BtnUp.key = ch;
	BtnDown.key = ch;
}