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
#include "gps.h"

GpsUnit::GpsUnit(LGFX_Sprite *spr) {
	sprite = spr;
}

bool GpsUnit::initSensor() {
#ifdef EMULATOR
	return true;
#endif
#ifdef M5STICK
	bool notFound = true;
	while (gpsRaw.available() > 0) {
		if (gps.encode((char) gpsRaw.read())) {
			notFound = false;
			break;
		}
	}
	return !notFound;
#endif
#ifdef PICOPAD
	bool notFound = true;
	while (Serial1.available() > 0) {
		if (gps.encode((char) Serial1.read())) {
			notFound = false;
			break;
		}
	}
	return !notFound;
#endif
}

void GpsUnit::drawSensorData(uint16_t background) {
	sprite->setTextColor(TFT_WHITE, background);
	sprite->setFont(&DSEG7_Classic_Bold_32);
	sprite->drawString(String(alt, 0).c_str(), 10, 30);

	sprite->fillRoundRect(10, 72, DISPLAY_WIDTH - 20, 46, 4, COLOR);
	sprite->fillRoundRect(10, 123, DISPLAY_WIDTH - 20, 46, 4, COLOR);

	sprite->setTextDatum(MC_DATUM);
	sprite->setTextColor(TFT_WHITE, COLOR);
	sprite->setTextFont(2);
	sprite->drawString(F("Latitude"), DISPLAY_WIDTH / 2, 82);
	sprite->drawString(F("Longitude"), DISPLAY_WIDTH / 2, 133);
	sprite->setFont(&DSEG7_Classic_Bold_17);
	sprite->drawString(String(lat, 6).c_str(), DISPLAY_WIDTH / 2, 102);
	sprite->drawString(String(lng, 6).c_str(), DISPLAY_WIDTH / 2, 153);

	sprite->setTextColor(TFT_WHITE, background);
	sprite->setTextFont(2);
	sprite->drawString(time, DISPLAY_WIDTH / 2, 183);
	sprite->drawString(date, DISPLAY_WIDTH / 2, 203);

	sprite->setTextColor(TFT_WHITE, COLOR);
	sprite->fillRoundRect(10, 217, DISPLAY_WIDTH - 20, 21, 4, COLOR);

	sprite->setTextDatum(TL_DATUM);
	sprite->setTextFont(2);
	sprite->drawString(F("Sat: "), 15, 220);
	sprite->drawString(String(satellites).c_str(), 45, 220);
	sprite->drawString(F("C: "), 70, 220);
	sprite->drawString(String(course, 0).c_str(), 84, 220);

	sprite->pushSprite(START_X, START_Y);
}

void GpsUnit::open() {
#ifdef M5STICK
	gpsRaw.begin(9600, SERIAL_8N1, 33, 32);
#endif
#ifdef PICOPAD
	Serial1.setRX(PORT_A_WHITE_PIN);
	Serial1.setTX(PORT_A_YELLOW_PIN);
	Serial1.setPollingMode(true);
	Serial1.begin(9600, SERIAL_8N1);
#endif

	bool connected = splash(F("GPS Unit"), F("Connect"), GpsUnitImg, 66, 131, true);

	if (connected) {
		unit_connected();

		while (!M5.BtnPWR.wasClicked()) {
			default_actions();

#ifdef M5STICK
			auto *pGpsSerial = &gpsRaw;
#endif
#ifdef PICOPAD
			auto *pGpsSerial = &Serial1;
#endif

#ifndef EMULATOR
			while (pGpsSerial->available() > 0) {
				char ch = (char) pGpsSerial->read();
				Serial.print(ch);
				if (gps.encode(ch)) {
					Serial.println();
					break;
				}
			}

			satellites = gps.satellites.value();
			if (gps.time.hour() > 23 || gps.time.minute() > 60 || gps.time.second() > 60) {
				stpcpy(time, "00:00:00");
			} else {
				snprintf(time, 9, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
			}
			if (gps.date.day() > 31 || gps.date.month() > 12 || gps.date.year() < 2024) {
				stpcpy(date, "00.00.0000");
			} else {
				snprintf(date, 11, "%02d.%02d.%04d", gps.date.day(), gps.date.month(), gps.date.year());
			}

			if (gps.location.isValid()) {
				lat = gps.location.lat();
				lng = gps.location.lng();
				alt = gps.altitude.meters();
				course = gps.course.deg();
			} else {
				lat = 0;
				lng = 0;
				alt = 0;
				course = 0;
			}
#else
			satellites = 0;
			lat = 0;
			lng = 0;
			alt = 0;
			course = 0;
			stpcpy(time, "00:00:00");
			stpcpy(date, "00.00.0000");
#endif
			drawAll(F("Altitude m"), TFT_BLACK, false);
			M5.update();
		}
	}
#ifndef EMULATOR
	Wire.end();
#endif
#ifdef M5STICK
	gpsRaw.end(true);
#endif
#ifdef PICOPAD
	Serial1.end();
#endif
}
