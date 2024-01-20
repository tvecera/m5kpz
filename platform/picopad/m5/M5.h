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
#ifndef PICO_M5STACK_M5_H
#define PICO_M5STACK_M5_H

#include "picopad.h"
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
		lgfx::Panel_ST7789 _panel_instance;
		lgfx::Bus_SPI _bus_instance;
		lgfx::Light_PWM _light_instance;

public:
		LGFX() {
			{
				auto cfg = _bus_instance.config();
				cfg.spi_host = 0;
				cfg.spi_mode = 2;
				cfg.freq_write = 62500000;
				cfg.pin_sclk = DISP_SCK_PIN;
				cfg.pin_mosi = DIDP_MOSI_PIN;
				cfg.pin_miso = -1;
				cfg.pin_dc = DISP_DC_PIN;
				_bus_instance.config(cfg);
				_panel_instance.setBus(&_bus_instance);
			}

			{
				auto cfg = _panel_instance.config();

				cfg.pin_cs = DISP_CS_PIN;
				cfg.pin_rst = DISP_RES_PIN;
				cfg.pin_busy = -1;

				cfg.panel_width = 240;
				cfg.panel_height = 320;
				cfg.offset_x = 0;
				cfg.offset_y = 0;
				cfg.offset_rotation = 1;
				cfg.dummy_read_pixel = 8;
				cfg.dummy_read_bits = 1;
				cfg.readable = false;
				cfg.invert = true;
				cfg.rgb_order = false;
				cfg.dlen_16bit = false;
				cfg.bus_shared = false;

				_panel_instance.config(cfg);
			}

			setPanel(&_panel_instance);
		}
};

class M5Button_Class {
public:
		explicit M5Button_Class(int type);
		int key = NOKEY;

		bool wasClicked();

private:
		int type;
};


class M5Display_Class {
public:
		void setBrightness(uint8_t brightness);
};

class M5Power_Class {
public:
		static int getBatteryVoltage();
};

class M5Common {
public:
		M5Common();

		M5Display_Class Display;
		M5Power_Class Power;

		M5Button_Class BtnA = M5Button_Class(KEY_A);
		M5Button_Class BtnB = M5Button_Class(KEY_B);
		M5Button_Class BtnPWR = M5Button_Class(KEY_X);
		M5Button_Class BtnY = M5Button_Class(KEY_Y);
		M5Button_Class BtnLeft = M5Button_Class(KEY_LEFT);
		M5Button_Class BtnRight = M5Button_Class(KEY_RIGHT);
		M5Button_Class BtnUp = M5Button_Class(KEY_UP);
		M5Button_Class BtnDown = M5Button_Class(KEY_DOWN);

		void update();
};

static M5Common M5;

#endif //PICO_M5STACK_M5_H
