/**
* MIT License
*
* Copyright (c) 2024 Tomas Vecera, tomas@vecera.dev
*
* This software is based on the Volos BrakeOut:
*   Repository: https://github.com/VolosR/M5StickCPlusBrakeout/tree/main
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
#include "brakeout.h"
#include "bmp.h"
#ifdef M5STICK
#include "joystick.h"
#endif

Brakeout::Brakeout(LGFX_Sprite *spr) {
	sprite = spr;
}

void Brakeout::draw() {
	if (fase == 0) {
		if (xj > 10 || xj < -10) {
			if (pom == 0) {
				pDisplay->fillScreen(TFT_BLACK);
				pDisplay->drawLine(START_X, 17, START_X, 240, TFT_GREY);
				pDisplay->drawLine(START_X, 17, START_X + 135, 17, TFT_GREY);
				pDisplay->drawLine(START_X + 134, 17, START_X + 134, 240, TFT_GREY);
				pDisplay->setTextFont(2);
				pDisplay->setCursor(START_X + 3, 3);
				pDisplay->setTextColor(TFT_WHITE, TFT_BLACK);
				pDisplay->setTextSize(1);
				pDisplay->setTextFont(2);
				pDisplay->setCursor(START_X, 0);
				pDisplay->println(("SCORE " + String(score)).c_str());
				pDisplay->setTextFont(2);
				pDisplay->setCursor(START_X + 99, 0);
				pDisplay->println(("LVL" + String(level)).c_str());
				fase = fase + 1;
				pom = 1;
			}
		} else {
			pom = 0;
		}
	}

	if (fase == 1) {
		if (y != float(ny)) {
			pDisplay->fillEllipse(int16_t(START_X + nx), int16_t(ny), 2, 2, TFT_BLACK);
			ny = int(y);
			nx = int(x);
		}

		if (int(px) != pxn) {
			pDisplay->fillRect(START_X + pxn, 234, 24, 4, TFT_BLACK);
			pxn = int(px);
		}

		if (px >= 2 && px <= 109) {
			if (xj > 10) {
				px = px - 1;
			}
			if (xj < -10) {
				px = px + 1;
			}
		}
		if (px <= 3) {
			px = 4;
		}

		if (px >= 108) {
			px = 107;
		}

		if (y > 232 && x > px && x < px + 24) {
			ys = ys * -1;
			xs = amount[RANDOM2(4)] * float(vrije[RANDOM2(2)]);
		}

		for (int j = 0; j < 16; j++) {
			if (x > float(enx[j]) && x < float(enx[j]) + 20 && y > float(eny[j]) && y < float(eny[j]) + 5) {
				pDisplay->fillRect(START_X + enx[j], eny[j], 20, 4, TFT_BLACK);
				enx[j] = 400;
				ys = ys * -1;
				xs = amount[RANDOM2(4)] * float(vrije[RANDOM2(2)]);
				score = score + 1;
				pDisplay->setTextFont(2);
				pDisplay->setCursor(START_X, 0);
				pDisplay->println(("SCORE " + String(score)).c_str());
			}
		}

		if (y < 21) {
			ys = ys * -1.00f;
		}

		if (y > 240) {
			fase = fase + 1;
		}

		if (x >= 130) {
			xs = xs * -1.00f;
		}

		if (x <= 4) {
			xs = xs * -1.00f;
		}

		// draw enemies
		for (int i = 0; i < 16; i++) {
			pDisplay->fillRect(START_X + enx[i], eny[i], 20, 4, enc[i]);
		}

		// draw ball
		pDisplay->fillEllipse(int16_t(START_X + x), int16_t(y), 2, 2, TFT_WHITE);

		//if(spe>10){  //change coordinates of ball
		y = y + ys;
		x = x + xs;
		//spe=0;
		//}

		pDisplay->fillRect(int32_t(START_X + px), 234, 24, 4, TFT_WHITE);

		if (score == 16 || score == 33 || score == 50 || score == 67 || score == 84 || score == 101 || score == 118 ||
				score == 135 || score == 152 || score == 169) {
			newLevel();
		}
		delayMicroseconds(gameSpeed);
	}

	if (fase == 2) {
		if (!rstFlag) {
			pDisplay->fillScreen(TFT_BLACK);
			rstFlag = true;
		}
		pDisplay->setTextDatum(TL_DATUM);
		pDisplay->setTextFont(2);
		pDisplay->setCursor(START_X + 13, 103);
  	pDisplay->setTextColor(TFT_WHITE, TFT_BLACK);
		pDisplay->setTextSize(1);
		pDisplay->println(F("GAME OVER"));
		pDisplay->setTextFont(4);
		pDisplay->setCursor(START_X + 13, 123);
		pDisplay->println(("SCORE:" + String(score)).c_str());
		pDisplay->setTextFont(4);
		pDisplay->setCursor(START_X + 13, 153);
		pDisplay->println(("LEVEL:" + String(level)).c_str());
		pDisplay->setTextFont(4);
		pDisplay->setCursor(START_X + 13, 123);
		pDisplay->println(("SCORE:" + String(score)).c_str());
		delay(30);
		if (M5.BtnA.wasClicked() || M5.BtnPWR.wasClicked()) {
			rstFlag = false;
			reset();
		}
	}
}

void Brakeout::reset() {
	score = 0;
	fase = 0;
	gameSpeed = gameSpeed - GAME_SPEED_STEP;
	level = 1;
	pDisplay->setTextFont(2);
	pDisplay->setCursor(START_X + 99, 0);
	pDisplay->println(("LVL" + String(level)).c_str());
	y = 75;
	ys = 1;
	gameSpeed = GAME_SPEED_DEFAULT;
	x = RANDOM(30, 100);

	int enx2[16] = {8, 33, 58, 83, 108, 8, 33, 58, 83, 108, 22, 47, 72, 97, 47, 72};
	for (int n = 0; n < 16; n++) {
		enx[n] = enx2[n];
	}
}

void Brakeout::newLevel() {
	score = score + 1;
	delay(3000);
	gameSpeed = gameSpeed - GAME_SPEED_STEP;
	level = level + 1;
	pDisplay->setTextFont(2);
	pDisplay->setCursor(START_X + 99, 0);
	pDisplay->println(("LVL" + String(level)).c_str());
	y = 75;
	ys = 1;
	x = RANDOM(30, 100);

	int enx2[16] = {8, 33, 58, 83, 108, 8, 33, 58, 83, 108, 22, 47, 72, 97, 47, 72};
	for (int n = 0; n < 16; n++) {
		enx[n] = enx2[n];
	}
}

bool Brakeout::initSensor() {
#ifdef M5STICK
	auto *instance = static_cast<Joystick *>(pJoystick);
	return instance->begin();
#else
	return true;
#endif
}

void Brakeout::open() {
#ifdef M5STICK
	auto *joystick = new Joystick();
	pJoystick = joystick;
#endif

	bool connected = splash(F("Brake Out Game"), F("Run"), BrakeoutUnitImg, 100, 100, true);

	if (connected) {
		unit_connected();
		pDisplay = sprite->getParent();
		pDisplay->setSwapBytes(true);

#ifdef M5STICK
		pDisplay->setRotation(2);
#else
		unsigned long timeout = millis();
#endif
		pDisplay->pushImage(START_X, 0, 135, 240, bootlogo);

		while (!M5.BtnPWR.wasClicked()) {
			default_actions();
#ifdef M5STICK
			xj = joystick->getX();
#endif
#ifdef EMULATOR
			if (M5.BtnC.wasClicked() || M5.BtnA.wasClicked()) {
				if (M5.BtnC.wasClicked()) { xj = -50; }
				if (M5.BtnA.wasClicked()) { xj = 50; }
				timeout = millis();
			} else {
				if ((millis() - timeout) > 200) {
					xj = 0;
					timeout = millis();
				}
			}
#endif
#ifdef PICOPAD
			if (M5.BtnLeft.wasClicked() || M5.BtnRight.wasClicked()) {
				if (M5.BtnRight.wasClicked()) { xj = -50; }
				if (M5.BtnLeft.wasClicked()) { xj = 50; }
				timeout = millis();
			} else {
				if ((millis() - timeout) > 200) {
					xj = 0;
					timeout = millis();
				}
			}
#endif

			draw();
			M5.update();
		}
		pDisplay->setSwapBytes(false);
#ifdef M5STICK
		pDisplay->setRotation(0);
#endif
	}
#ifdef M5STICK
	joystick->end();
	delete joystick;
#endif
}
