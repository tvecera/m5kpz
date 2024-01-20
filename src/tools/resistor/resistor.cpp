/**
* MIT License
*
* Copyright (c) 2024 Tomas Vecera, tomas@vecera.dev
*
* This software is based on the IcingTomato M5Stick C Plus colour-ring resistor calculator:
*   Repository: https://github.com/IcingTomato/M5StickC-Resistor-Calculator
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
#include "resistor.h"
#include "res.h"

Resistor::Resistor(LGFX_Sprite *spr) {
	sprite = spr;
}

void Resistor::open() {

	bool connected = splash(F("Resistor CALC"), F("Run"), ResistorUnitImg, 100, 54, false);

	if (connected) {
		sprite->fillScreen(TFT_BLACK);
		sprite->pushSprite(START_X, START_Y);
		sprite->setRotation(1);

		sprite->fillScreen(TFT_BLACK);
		sprite->setTextFont(2);
		sprite->setTextDatum(TL_DATUM);
		sprite->drawString(F("Resistor CALC"), x, 0);
		sprite->pushImage(x, y, 120, 26, resistor);

		sprite->fillRect(x + 30, y, 6, 26, colors[first]);
		sprite->fillRect(x + 42, y + 3, 6, 20, colors[secon]);
		sprite->fillRect(x + 54, y + 3, 6, 20, colors[multiplier]);
		sprite->fillRect(x + 82, y, 6, 26, cT[tolerance]);

		sprite->fillCircle(bPos[band] + 2, 58, 2, TFT_WHITE);
		for (int i = 0; i < 12; i++) {
			if (i < 10) {
				sprite->fillRect(0, i * 13, 11, 13, colors[i]);
				sprite->fillRect(22, i * 13, 11, 13, colors[i]);
				if (i == 0) {
					sprite->setTextColor(TFT_WHITE, colors[i]);
				} else {
					sprite->setTextColor(TFT_BLACK, colors[i]);
				}

				sprite->setTextFont(1);
				sprite->drawString(String(i).c_str(), 3, (i * 13) + 2);
				sprite->drawString(String(i).c_str(), 25, (i * 13) + 2);
				sprite->setTextColor(TFT_WHITE, TFT_BLACK);
			}
			if (i == 0) {
				sprite->drawCircle(235, (i * 11) + 5, 4, TFT_WHITE);
			} else {
				sprite->fillCircle(235, (i * 11) + 5, 4, colors[i]);
			}
			sprite->setTextFont(1);
			sprite->drawString(mm[i].c_str(), 194, (i * 11) + 1);
		}

		sprite->fillCircle(16, (first * 13) + 5, 2, TFT_WHITE);
		sprite->fillCircle(38, (secon * 13) + 5, 2, TFT_WHITE);
		sprite->fillCircle(188, (multiplier * 11) + 5, 2, TFT_WHITE);

		for (int i = 0; i < 4; i++) {
			sprite->fillRect(44 + (i * 34), 117, 34, 11, cT[i]);
			sprite->setTextColor(TFT_BLACK, cT[i]);
			sprite->setTextFont(1);
			sprite->drawString((String(tt[i]) + "%").c_str(), 54 + (i * 34), 119);
		}
		sprite->fillCircle(61 + (tolerance * 34), 133, 2, TFT_WHITE);
		sprite->setTextFont(1);
		sprite->setTextColor(TFT_WHITE, TFT_BLACK);

		while (!M5.BtnPWR.wasClicked()) {
			if (M5.BtnA.wasClicked()) {
				if (deb1 == 0) {
					sprite->fillCircle(bPos[band] + 2, 58, 2, TFT_BLACK);
					deb1 = 1;
					band++;
					if (band > 3) {
						band = 0;
					}
					sprite->fillCircle(bPos[band] + 2, 58, 2, TFT_WHITE);
				}
			} else {
				deb1 = 0;
			}

			if (M5.BtnB.wasClicked()) {
				if (deb2 == 0) {
					deb2 = 1;
					if (band == 0) {
						sprite->fillCircle(16, (first * 13) + 5, 2, TFT_BLACK);
						first++;
						if (first > 9) {
							first = 0;
						}
						sprite->fillRect(x + 30, y, 6, 26, colors[first]);
						sprite->fillCircle(16, (first * 13) + 5, 2, TFT_WHITE);
					}
					if (band == 1) {
						sprite->fillCircle(38, (secon * 13) + 5, 2, TFT_BLACK);
						secon++;
						if (secon > 9) {
							secon = 0;
						}
						sprite->fillCircle(38, (secon * 13) + 5, 2, TFT_WHITE);
						sprite->fillRect(x + 42, y + 3, 6, 20, colors[secon]);
					}

					if (band == 2) {
						sprite->fillCircle(188, (multiplier * 11) + 5, 2, TFT_BLACK);
						multiplier++;
						if (multiplier > 11) {
							multiplier = 0;
						}
						sprite->fillCircle(188, (multiplier * 11) + 5, 2, TFT_WHITE);
						sprite->fillRect(x + 54, y + 3, 6, 20, colors[multiplier]);
					}

					if (band == 3) {
						sprite->fillCircle(61 + (tolerance * 34), 133, 2, TFT_BLACK);
						tolerance++;
						if (tolerance > 3) {
							tolerance = 0;
						}
						sprite->fillCircle(61 + (tolerance * 34), 133, 2, TFT_WHITE);
						sprite->fillRect(x + 82, y, 6, 26, cT[tolerance]);
					}

					double res = ((first * 10) + secon) * multis[multiplier];
					sprite->fillRect(44, 70, 136, 46, 0x00A5);
					double maxx = res + (res * (tt[tolerance] / 100.00));
					double minn = res - (res * (tt[tolerance] / 100.00));
					sprite->setTextColor(TFT_WHITE, 0x00A5);

					if (res >= 1000 && res < 1000000) {
						sprite->setTextFont(2);
						sprite->drawString((String(first) + String(secon) + String("x10 =") + String(res / 1000) + String(" K")).c_str(), 48, 76);
						sprite->setTextFont(1);
						sprite->drawString(String(multiplier).c_str(), 85, 72);
						sprite->drawString(("MAX:" + String(maxx)).c_str(), 48, 94);
						sprite->drawString(("MIN:" + String(minn)).c_str(), 48, 106);
					} else if (res >= 1000000) {
						sprite->setTextFont(2);
						sprite->drawString((String(first) + String(secon) + String("x10 =") + String(res / 100000) + String(" M")).c_str(), 48, 76);
						sprite->setTextFont(1);
						sprite->drawString(String(multiplier).c_str(), 85, 72);
						sprite->drawString(("MAX:" + String(maxx)).c_str(), 48, 94);
						sprite->drawString(("MIN:" + String(minn)).c_str(), 48, 106);
					} else {
						sprite->setTextFont(2);
						sprite->drawString((String(first) + String(secon) + String("x10 =") + String(res) + String(" ohm")).c_str(), 48, 76);
						sprite->setTextFont(1);
						sprite->drawString(String(multiplier).c_str(), 85, 72);
						sprite->drawString(("MAX:" + String(maxx)).c_str(), 48, 94);
						sprite->drawString(("MIN:" + String(minn)).c_str(), 48, 106);
					}
					sprite->setTextColor(TFT_WHITE, TFT_BLACK);
				}
			} else {
				deb2 = 0;
			}
#ifdef M5STICK
			sprite->pushSprite(START_X, START_Y);
#else
			sprite->pushRotated(-90);
#endif
			M5.update();
		}
		sprite->fillScreen(TFT_BLACK);
#ifdef M5STICK
		sprite->pushSprite(START_X, START_Y);
#else
		sprite->pushRotated(-90);
#endif
		sprite->setRotation(0);
	}
}
