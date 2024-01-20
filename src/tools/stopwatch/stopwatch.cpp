/**
* MIT License
*
* Copyright (c) 2024 Tomas Vecera, tomas@vecera.dev
*
* This software is based on the Volos StopWatch:
*   Repository: https://github.com/VolosR/M5StickStopwatch
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
#include "stopwatch.h"
#include "DSEG7_Classic_Regular_16.h"
#include "stop.h"
#include "play.h"

#define GREY 0x65DB
#define color 0xF251

#ifdef M5STICK
#define LEFT_BUTTON 32
#define RIGHT_BUTTON 33
#endif

Stopwatch::Stopwatch(LGFX_Sprite *spr) {
	sprite = spr;
}

void Stopwatch::reset() {
	sprite->fillScreen(TFT_BLACK);
	sprite->pushImage(7, 75, 48, 48, stop);
	sprite->pushImage(30, 136, 72, 72, play);
	sprite->setTextFont(2);
	sprite->setTextDatum(TL_DATUM);
	sprite->drawString(F("STOPWATCH"), 14, 7);
	sprite->drawString(("BAT " + String(vbat) + "V").c_str(), 15, 222);

	for (int i = 0; i < 5; i++)
		sprite->fillRect(100 + (i * 6), 224, 4, 12, TFT_BLACK);
	for (int i = 0; i < chosenB + 1; i++)
		sprite->fillRect(100 + (i * 6), 224, 4, 12, color);

	sprite->setFont(&DSEG7_Classic_Regular_32);
	sprite->drawLine(0, 220, 128, 220, GREY);
	laps = 0;
	lss[0] = 0;
	ls[0] = 0;
	lm[0] = 0;
	ss = 0;
	s = 0;
	m = 0;
	fase = 0;
	winner = 0;
}

void Stopwatch::buttons() {

#ifdef M5STICK
	leftButton = digitalRead(LEFT_BUTTON) == 0;
	rightButton = digitalRead(RIGHT_BUTTON) == 0;
#endif
#ifdef PICOPAD
	leftButton = M5.BtnLeft.wasClicked();
	rightButton = M5.BtnRight.wasClicked();
#endif
#ifdef EMULATOR
	leftButton = M5.BtnA.wasClicked();
	rightButton = M5.BtnC.wasClicked();
#endif

	if (leftButton) {
		if (pom == 0) {
			if (fase == 1) {
				winner++;
				laps++;
				if (winner == 1) {
					sprite->fillRect(15, 130, 120, 80, TFT_BLACK);
					sprite->setTextFont(2);
					sprite->drawString(F("LAP:"), 20, 130);
				}
				if (winner > 3) {
					sprite->fillRect(15, 144, 120, 70, TFT_BLACK);
					winner = 1;
				}
				sprite->setFont(&DSEG14_Classic_Mini_Regular_15);
				lss[laps] = ss;
				ls[laps] = s;
				lm[laps] = m;

				if ((lm[laps] - lm[laps - 1]) < 10)
					minutesL = "0" + String(lm[laps] - lm[laps - 1]);
				else
					minutesL = String(lm[laps] - lm[laps - 1]);

				if ((ls[laps] - ls[laps - 1]) < 10)
					secondsL = "0" + String(ls[laps] - ls[laps - 1]);
				else
					secondsL = String(ls[laps] - ls[laps - 1]);

				int lsss = lss[laps] - lss[laps - 1];
				if (lsss < 0)
					lsss = lsss * -1;

				if (lsss < 10)
					milisL = "0" + String(lsss);
				else
					milisL = String(lsss);

				lapTime[laps - 1] = minutesL + " : " + secondsL + " : " + milisL;
				sprite->drawString(lapTime[laps - 1].c_str(), 18, 130 + winner * 22);
			}

			if (fase == 0)
				fase = 1;
			pom = 1;
		}
	} else {
		pom = 0;
	}

	if (rightButton) {
		if (pom2 == 0) {
			fase++;
			if (fase == 3) {

				sprite->fillScreen(TFT_BLACK);
				sprite->setTextFont(2);
				sprite->drawString(F("TIME:"), 10, 6);
				sprite->drawString(F("LAP:"), 10, 50);
				sprite->setFont(&DSEG14_Classic_Mini_Regular_15);
				sprite->drawString((current + ":" + milis).c_str(), 10, 28);
				for (int i = 0; i < laps; i++) {
					sprite->setTextFont(2);
					sprite->drawString((String(i + 1) + ".   " + lapTime[i]).c_str(), 10, 68 + (14 * i));
				}
				for (int i = 0; i < 5; i++)
					sprite->fillRect(100 + (i * 6), 224, 4, 12, TFT_BLACK);
				for (int i = 0; i < chosenB + 1; i++)
					sprite->fillRect(100 + (i * 6), 224, 4, 12, color);

				sprite->drawLine(0, 220, 128, 220, GREY);
			}
			if (fase > 3) {
				reset();
			}
			pom2 = 1;
		}
	} else {
		pom2 = 0;
	}
}

void Stopwatch::open() {
	bool connected = splash(F("Stopwatch"), F("Run"), StopwatchUnitImg, 100, 100, false);

	if (connected) {
		unit_connected();

#ifdef M5STICK
		M5.Display.setSwapBytes(true);
		pinMode(LEFT_BUTTON, INPUT_PULLUP);
		pinMode(RIGHT_BUTTON, INPUT_PULLUP);
#endif
		reset();

		while (!M5.BtnPWR.wasClicked()) {
#ifdef PICOPAD
			if (M5.BtnY.wasClicked()) {
				reset_to_boot_loader();
			}
#endif
			if (M5.BtnB.wasClicked()) {
				chosenB++;
				if (chosenB == 5) chosenB = 0;
				M5.Display.setBrightness(brightness[chosenB]);

				for (int i = 0; i < 5; i++)
					sprite->fillRect(100 + (i * 6), 224, 4, 12, TFT_BLACK);
				for (int i = 0; i < chosenB + 1; i++)
					sprite->fillRect(100 + (i * 6), 224, 4, 12, color);
			}
#ifndef EMULATOR
			vbat = float(M5.Power.getBatteryVoltage()) / 1000.0f;
#else
			vbat = 4.15f;
#endif

			sprite->setTextFont(2);
			sprite->drawString(("BAT " + String(vbat) + "V").c_str(), 15, 222);

			if (fase < 3) {
				tt = millis();
				sprite->setFont(&DSEG7_Classic_Regular_32);
				if (s < 10) seconds = "0" + String(s); else seconds = String(s);
				if (m < 10) minutes = "0" + String(m); else minutes = String(m);
				current = minutes + ":" + seconds;

				if (current != temp) {
					sprite->drawString(current.c_str(), 10, 30);
					temp = current;
				}

				if (ss < 10) milis = "0" + String(ss); else milis = String(ss);
				sprite->setTextFont(7);
				sprite->drawString(milis.c_str(), 60, 76);
			}

			if (tt > tt2 + 8) {
				tt2 = tt;
				if (fase == 1)
					ss++;
			}

			if (ss > 99) {
				s++;
				ss = 0;
			}
			if (s > 59) {
				s = 0;
				m++;
			}
			M5.update();
			buttons();
			sprite->pushSprite(START_X, START_Y);
		}
	}
#ifdef M5STICK
	M5.Display.setSwapBytes(false);
#endif
}
