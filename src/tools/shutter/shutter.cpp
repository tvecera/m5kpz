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
#ifndef PICOPAD

#include "shutter.h"

Shutter::Shutter(LGFX_Sprite *spr) {
	sprite = spr;
}

void Shutter::drawSensorData(uint16_t background) {
	sprite->fillCircle(DISPLAY_WIDTH / 2, (DISPLAY_HEIGHT / 2) - 20, 40, mobileConnected ? TFT_DARKGREEN : TFT_RED);
	sprite->setTextDatum(MC_DATUM);
	sprite->setTextColor(TFT_WHITE, mobileConnected ? TFT_DARKGREEN : TFT_RED);
	sprite->setTextFont(4);
	sprite->drawString(mobileConnected ? F("ON") : F("OFF"), DISPLAY_WIDTH / 2, (DISPLAY_HEIGHT / 2) - 15);
}

void Shutter::open() {
	bool connected = splash(F("Camera Shutter"), F("Run"), ShutterUnitImg, 100, 75, false);

	if (connected) {
		unit_connected();

#ifdef M5STICK
		bleKeyboard.begin();
		M5.Speaker.begin();
#endif

		while (!M5.BtnPWR.wasClicked()) {
			default_actions();

#ifdef M5STICK
			if (M5.BtnA.wasClicked()) {
				Beep();
				bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
			}
			mobileConnected = bleKeyboard.isConnected();
#endif
#ifdef EMULATOR
			if (M5.BtnA.wasClicked()) {
				mobileConnected = !mobileConnected;
			}
#endif
			drawAll(F("Camera"));
			M5.update();
		}

#ifdef M5STICK
		bleKeyboard.end();
		M5.Speaker.end();
		ESP.restart();
#endif
	}
}

#endif