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
#ifndef STICK_API_H
#define STICK_API_H

#ifdef PICOPAD
#include <LovyanGFX.hpp>
#include "picopad.h"
#include "FreeRTOS.h"
#include <semphr.h>
#include "M5.h"
#else
#include <M5Unified.h>
#endif
#ifdef EMULATOR
#if EMULATOR
#include "avr_stdlib.h"
#include "pgmspace.h"
#include "WString.h"
#endif
#endif

#ifndef EMULATOR
#include "Wire.h"
#endif
#include "fonts.h"

#define DEFAULT_BRG 2

#define COLOR 0x01EA
#define COLOR2 TFT_ORANGE
#define TFT_GOLD 0xFEA0
#define TFT_BROWN 0x9A60
#define TFT_VIOLET 0x915C
#define TFT_SILVER 0xC618
#define TFT_TIDE 0x0083
#define TFT_GREY 0x5AEB

#define DISPLAY_WIDTH 135
#define DISPLAY_HEIGHT 240
#ifdef M5STICK
#define START_X 0
#define START_Y 0
#else
#define START_X 92
#define START_Y 0
#endif

#ifdef PICOPAD
#define M5_LED LED1_PIN
#define PORT_A_SDA_PIN 0
#define PORT_A_SCL_PIN 1
#else
#define M5_IR 9
#define M5_LED 10
#define PORT_A_SDA_PIN 32
#define PORT_A_SCL_PIN 33
#endif

#ifdef M5STICK
#endif
#ifdef M5STICK
#define M5_LED 10
#endif

#ifdef EMULATOR
#define RANDOM(howsmall, howbig) (howsmall + rand() % (howbig - howsmall))
#define RANDOM2(howbig) (rand() % howbig)
#else
#define RANDOM(howsmall, howbig) random(howsmall, howbig)
#define RANDOM2(howbig) random(howbig)
#endif

#define PORT_A_YELLOW_PIN PORT_A_SDA_PIN
#define PORT_A_WHITE_PIN PORT_A_SCL_PIN

#define PlayTone(freq, dur) M5.Speaker.tone(freq, dur)
#define Beep() PlayTone(4000, 100)

class IUnit {
public:
		virtual void open() = 0;

		virtual ~IUnit() = default;

private:
		virtual void drawSensorData(uint16_t background) {};

		virtual bool initSensor() { return true; };

protected:
		LGFX_Sprite *sprite = nullptr;

#ifndef PICOPAD
		const uint8_t brightness[5] = {60, 80, 100, 110, 127};
#else
		const uint8_t brightness[5] = {15, 31, 63, 127, 255};
#endif
		uint8_t chosenB = DEFAULT_BRG;
		float vbat = 0.0;

		uint8_t get_chosen_brightness() {
			return brightness[chosenB];
		}

		void unit_connected() {

			M5.Display.setBrightness(get_chosen_brightness());
			sprite->fillSprite(TFT_BLACK);
			sprite->pushSprite(START_X, START_Y);
			M5.update();
		}

		void default_actions() {
#ifdef PICOPAD
			if (M5.BtnY.wasClicked()) {
				reset_to_boot_loader();
			}
#endif
			if (M5.BtnB.wasClicked()) {
				chosenB++;
				if (chosenB == 5) chosenB = 0;
				M5.Display.setBrightness(brightness[chosenB]);
			}
#ifndef EMULATOR
			vbat = float(M5.Power.getBatteryVoltage()) / 1000.0f;
#else
			vbat = 4.15f;
#endif
		}

		static bool
		drawSplash(LGFX_Sprite *sprite, const String &title, const String &subtitle, const uint16_t *image,
							 uint8_t image_width,
							 uint8_t image_height) {
			sprite->fillSprite(TFT_BLACK);
			sprite->pushSprite(START_X, START_Y);

			bool connected = false;
			bool next = false;
			while (!connected && !next) {
				if (M5.BtnA.wasClicked()) {
					connected = true;
					next = false;
				}
				if (M5.BtnB.wasClicked()) {
					connected = false;
					next = true;
				}
#ifdef PICOPAD
				if (M5.BtnY.wasClicked()) {
					reset_to_boot_loader();
				}
#endif

				sprite->setTextDatum(MC_DATUM);
				sprite->setTextColor(TFT_ORANGE, TFT_BLACK);
				sprite->setTextFont(2);
				sprite->drawString(title.c_str(), DISPLAY_WIDTH / 2, 30);
				sprite->setTextColor(TFT_WHITE, TFT_BLACK);
				sprite->setTextFont(2);
				sprite->drawString(subtitle.c_str(), DISPLAY_WIDTH / 2, 60);
				sprite->pushImage((DISPLAY_WIDTH / 2) - (image_width / 2), 90, image_width, image_height, image);
				sprite->pushSprite(START_X, START_Y);
				M5.update();
			}

			return connected && !next;
		}

		bool splash(const String &title, const String &subtitle, const uint16_t *image, uint8_t image_width,
								uint8_t image_height, bool checkSensor) {

			bool found = false;
			bool connected;
			if (checkSensor) {
				while (!found) {
					connected = drawSplash(sprite, title, subtitle, image, image_width, image_height);

					if (connected) {
						found = initSensor();
					} else {
						break;
					}
				}
			} else {
				connected = drawSplash(sprite, title, subtitle, image, image_width, image_height);
			}

			return connected;
		}

		void drawAll(const String &title, uint16_t background = TFT_BLACK, bool showBat = true, bool small = false) {
			sprite->fillSprite(background);

			for (int i = 0; i < 5; i++)
				sprite->fillRect((DISPLAY_WIDTH - 35) + (i * 6), 10, 4, 12, COLOR);

			for (int i = 0; i < chosenB + 1; i++)
				sprite->fillRect((DISPLAY_WIDTH - 35) + (i * 6), 10, 4, 12, COLOR2);

			if (background != TFT_BLACK) {
				sprite->setTextColor(TFT_BLACK, background);
			} else {
				sprite->setTextColor(TFT_WHITE, TFT_BLACK);
			}
			sprite->setTextDatum(TL_DATUM);
			sprite->setTextFont(2);
			sprite->drawString(title.c_str(), 10, 8);

			drawSensorData(background);

			if (showBat) {
				if (background != TFT_BLACK) {
					sprite->setTextColor(TFT_BLACK, background);
				} else {
					sprite->setTextColor(TFT_WHITE, TFT_BLACK);
				}

				if (small) {
					sprite->setTextDatum(MC_DATUM);
					sprite->setTextFont(2);
					sprite->drawString(("Battery: " + String(vbat) + " V").c_str(), DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 20);
				} else {
					sprite->setTextDatum(MC_DATUM);
					sprite->setTextFont(2);
					sprite->drawString(F("BATTERY V"), DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 40);
					sprite->setFont(&DSEG7_Classic_Bold_17);
					sprite->drawString(String(vbat).c_str(), DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 20);
				}
			}

			sprite->pushSprite(START_X, START_Y);
		}

#ifdef EMULATOR
		static uint32_t millis() {
			return SDL_GetTicks();
		}

		static void delay(uint32_t millis) {
			SDL_Delay(millis);
		}

		static void delayMicroseconds(uint32_t millis) {
			SDL_Delay(millis);
		}

#endif
};

#endif //STICK_API_H
