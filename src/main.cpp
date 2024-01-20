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
#include "api.h"

// Sensors
#include "sensors/enviii/enviii.h"
#include "sensors/ncir/ncir.h"
#include "sensors/tvoc/tvoc.h"
#include "sensors/tof/tof.h"
#include "sensors/gps/gps.h"
#include "sensors/pir/pir.h"
#include "sensors/voltmeter/voltmeter.h"
#include "sensors/irdecoder/irdecoder.h"

// Tools
#include "tools/torch/torch.h"
#include "tools/flashlight/flashlight.h"
#ifndef PICOPAD
#include "tools/shutter/shutter.h"
#endif
#include "tools/stopwatch/stopwatch.h"
#include "tools/resistor/resistor.h"
#include "tools/tvremote/tvremote.h"

// Games
#include "games/dices/dices.h"
#include "games/brakeout/brakeout.h"
#include "games/tetris/tetris.h"

LGFX_Sprite *spr = nullptr;
#ifdef PICOPAD
LGFX display;
#endif

int active = 0;
uint8_t len = 0;

typedef IUnit *(*CreateFunction)();

CreateFunction functions[20];

IUnit *createEnvIII() { return new EnvIII(spr); }
IUnit *createNcir() { return new Ncir(spr); }
IUnit *createTvoc() { return new Tvoc(spr); }
IUnit *createTof() { return new Tof(spr); }
IUnit *createGpsUnit() { return new GpsUnit(spr); }
IUnit *createPir() { return new Pir(spr); }
IUnit *createVoltmeter() { return new VoltmeterModule(spr); }
IUnit *createIRDecoder() { return new IRDecoder(spr); }
IUnit *creatStopwatch() { return new Stopwatch(spr); }
IUnit *createResistor() { return new Resistor(spr); }
IUnit *createTVRemote() { return new TVRemote(spr); }
IUnit *createTorch() { return new Torch(spr); }
IUnit *createFlashLight() { return new FlashLight(spr); }
IUnit *createDices() { return new Dices(spr); }
IUnit *createBrakeout() { return new Brakeout(spr); }
IUnit *createTetris() { return new Tetris(spr); }

#ifndef PICOPAD
IUnit *createShutter() { return new Shutter(spr); }
#endif

void reset() {
	M5.update();
	spr->setRotation(0);
	spr->setSwapBytes(true);
#ifndef PICOPAD
	M5.Display.setTextFont(1);
	M5.Display.setTextSize(1);
	M5.Display.setTextDatum(TL_DATUM);
	M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
#else
	StopSound();
	display.setTextFont(1);
	display.setTextSize(1);
	display.setTextDatum(TL_DATUM);
	display.setTextColor(TFT_WHITE, TFT_BLACK);
#endif

#ifndef EMULATOR
	Wire.end();
	digitalWrite(M5_LED, HIGH);
#endif

#ifdef M5STICK
	M5.Speaker.end();
	//gpio_reset_pin(GPIO_NUM_0);
	//gpio_reset_pin(GPIO_NUM_26);
	gpio_reset_pin(GPIO_NUM_10);
	gpio_reset_pin(GPIO_NUM_36);
	gpio_reset_pin(GPIO_NUM_32);
	gpio_reset_pin(GPIO_NUM_33);
	gpio_pulldown_dis(GPIO_NUM_25);
	gpio_pullup_dis(GPIO_NUM_25);
	pinMode(GPIO_NUM_0, INPUT_PULLDOWN);
	pinMode(GPIO_NUM_26, INPUT_PULLDOWN);
#endif
}

void setup() {
#ifdef PICOPAD
	Serial.begin(115200);
	delay(100);
	Serial.println("M5 application starting...");
	display.init();
	display.setEpdMode(epd_mode_t::epd_fastest);
	device_init();
	M5.Display.setBrightness(0);
	display.fillScreen(TFT_BLACK);
	M5.Display.setBrightness(63);
	spr = new LGFX_Sprite(&display);
#else
	M5.begin();
	M5.Speaker.end();
	spr = new LGFX_Sprite(&M5.Display);
	M5.Display.setBrightness(100);
	M5.Display.setEpdMode(epd_mode_t::epd_fastest);
#endif
	spr->createSprite(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	spr->setSwapBytes(true);

	uint8_t idx = 0;

	functions[idx++] = createEnvIII;
	functions[idx++] = createNcir;
	functions[idx++] = createTvoc;
	functions[idx++] = createTof;
	functions[idx++] = createGpsUnit;
	functions[idx++] = createPir;
	functions[idx++] = createVoltmeter;
	functions[idx++] = createIRDecoder;

	functions[idx++] = createTorch;
	functions[idx++] = createFlashLight;
#ifndef PICOPAD
	functions[idx++] = createShutter;
#endif
	functions[idx++] = creatStopwatch;
	functions[idx++] = createResistor;
	functions[idx++] = createTVRemote;

	functions[idx++] = createDices;
	functions[idx++] = createBrakeout;
	functions[idx++] = createTetris;

	len = idx;
}

void loop() {
	if (active < len) {
#ifdef M5STICK
		Serial.println(F("Init module"));
		Serial.print(F("[APP] Free memory: "));
		Serial.print(esp_get_free_heap_size());
		Serial.println(F(" bytes"));
		Serial.print(F("This task watermark: "));
		Serial.print(uxTaskGetStackHighWaterMark(nullptr));
		Serial.println(F(" bytes"));
#endif
		IUnit *module = functions[active]();
		if (module) {
			module->open();
			delete module;
#ifdef M5STICK
			Serial.print(F("[APP] Free memory: "));
			Serial.print(esp_get_free_heap_size());
			Serial.println(F(" bytes"));
			Serial.print(F("This task watermark: "));
			Serial.print(uxTaskGetStackHighWaterMark(nullptr));
			Serial.println(F(" bytes"));
#endif
		}
	}

	reset();
	active = active + 1;
	if (active >= len)
		active = 0;
}