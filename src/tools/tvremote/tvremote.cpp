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
#include "tvremote.h"
#ifndef EMULATOR
#define IR_SEND_PIN PORT_A_YELLOW_PIN
#include "IRremote.hpp"
#endif
#ifdef PICOPAD
#include "sound/alarm.c"
#endif

TVRemote::TVRemote(LGFX_Sprite *spr) {
	sprite = spr;
#ifdef M5STICK
	joystick = new Joystick();
#endif
}

bool TVRemote::initSensor() {
#ifdef M5STICK
	return joystick->begin();
#else
	return true;
#endif
}


void TVRemote::drawSensorData(uint16_t background) {
	sprite->fillRoundRect((135 - 55) / 2, 35, 55, 26, 4, COLOR);
	sprite->fillRoundRect(10, 35 + 26 + 5, 55, 26, 4, COLOR);
	sprite->fillRoundRect(70, 35 + 26 + 5, 55, 26, 4, COLOR);
	sprite->fillRoundRect((135 - 55) / 2, 35 + ((26 + 5) * 2), 55, 26, 4, COLOR);

	sprite->setTextDatum(MC_DATUM);
	sprite->setTextColor(TFT_WHITE, COLOR);
	sprite->setTextFont(2);
	sprite->drawString(F("UP"), DISPLAY_WIDTH / 2, 48);
	sprite->drawString(F("-"), 37, 79);
	sprite->drawString(F("+"), 97, 79);
	sprite->drawString(F("DOWN"), DISPLAY_WIDTH / 2, 110);

	sprite->setTextDatum(TL_DATUM);
	sprite->setTextColor(TFT_WHITE, background);
	sprite->setTextFont(2);
	for (int i = 0; i < 4; i++) {
		sprite->drawString(cc[i].c_str(), 45, 137 + (i * 15));
	}

	sprite->fillCircle(37, 145 + (chosen * 15), 4, TFT_RED);
}

void TVRemote::waitAndBlink() {
#ifndef EMULATOR
#ifdef M5STICK
	digitalWrite(M5_LED, LOW);  //LED ON
	Beep();
	delay(100);
	digitalWrite(M5_LED, HIGH);  //LED OFF
#endif
#ifdef PICOPAD
	digitalWrite(M5_LED, LOW);  //LED ON
	PLAYSOUND(AlarmSnd);
	delay(100);
	digitalWrite(M5_LED, HIGH);  //LED OFF
#endif
#endif
}

void TVRemote::open() {
	bool connected = splash(F("TV Remote"), F("Run"), TVRemoteUnitImg, 63, 130, true);

	if (connected) {
		unit_connected();
#ifndef EMULATOR
		auto *sender = new IRsend();
		sender->begin(false);
#ifdef M5STICK
		M5.Display.setRotation(2);
		M5.Speaker.begin();
#endif
#endif

		while (!M5.BtnPWR.wasClicked()) {
#ifndef PICOPAD
			default_actions();
#endif

#ifdef M5STICK
			chooseBtn = M5.BtnA.wasClicked();
			clickBtn = joystick->isClick();
			leftBtn = joystick->isLeft();
			rightBtn = joystick->isRight();
			upBtn = joystick->isUp();
			downBtn = joystick->isDown();
#endif
#ifdef PICOPAD
			if (M5.BtnY.wasClicked()) {
				reset_to_boot_loader();
			}
			vbat = float(M5.Power.getBatteryVoltage()) / 1000.0f;

			clickBtn = M5.BtnA.wasClicked();
			chooseBtn = M5.BtnB.wasClicked();
			leftBtn = M5.BtnLeft.wasClicked();
			rightBtn = M5.BtnRight.wasClicked();
			upBtn = M5.BtnUp.wasClicked();
			downBtn = M5.BtnDown.wasClicked();
#endif
#ifdef EMULATOR
			chooseBtn = M5.BtnA.wasClicked();
#endif
			// Change mode
			if (chooseBtn) {
				chosen++;
				if (chosen == 4) {
					chosen = 0;
				}
			}

#ifndef EMULATOR
			else if (clickBtn) {
				clickBtn = false;
				// ENTER
				sender->sendSamsung(0x707, 0x68, 1);
				waitAndBlink();
			} else {
				if (leftBtn) {
					leftBtn = false;
					switch (chosen) {
						case 0:
							// LEFT
							sender->sendSamsung(0x707, 0x65, 1);
							waitAndBlink();
							break;
						case 1:
							// Volume DOWN
							sender->sendSamsung(0x707, 0xB, 1);
							waitAndBlink();
							break;
						case 2:
							// Channel DOWN
							sender->sendSamsung(0x707, 0x10, 1);
							waitAndBlink();
							break;
						case 3:
							// Power ON/OF
							sender->sendSamsung(0x707, 0x02, 1);
							waitAndBlink();
							break;
					}
				} else if (rightBtn) {
					rightBtn = false;
					// RIGHT
					switch (chosen) {
						case 0:
							// RIGHT
							sender->sendSamsung(0x707, 0x62, 1);
							waitAndBlink();
							break;
						case 1:
							// Volume UP
							sender->sendSamsung(0x707, 0x7, 1);
							waitAndBlink();
							break;
						case 2:
							// Channel UP
							sender->sendSamsung(0x707, 0x12, 1);
							waitAndBlink();
							break;
						case 3:
							// Power ON/OF
							sender->sendSamsung(0x707, 0x02, 1);
							waitAndBlink();
							break;
					}
				} else if (downBtn) {
					downBtn = false;
					// DOWN
					if (chosen == 0) {
						sender->sendSamsung(0x707, 0x61, 1);
						waitAndBlink();
					}
				} else if (upBtn) {
					upBtn = false;
					switch (chosen) {
						case 0:
							// UP
							sender->sendSamsung(0x707, 0x61, 1);
							waitAndBlink();
							break;
						case 3:
							// Return
							sender->sendSamsung(0x707, 0x58, 1);
							waitAndBlink();
							break;
					}
				}
			}
#endif

			delay(1);
			drawAll(F("Samsung"), TFT_BLACK, true, true);
			M5.update();
		}
#ifndef EMULATOR
#ifdef M5STICK
		M5.Speaker.end();
		M5.Display.setRotation(0);
		joystick->end();
#endif
		delete sender;
		Wire.end();
#endif
	}
#ifdef M5STICK
	delete joystick;
#endif
}
