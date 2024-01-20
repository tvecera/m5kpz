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
#include "irdecoder.h"

#ifndef EMULATOR
#define DECODE_DENON
#define DECODE_JVC
#define DECODE_KASEIKYO
#define DECODE_PANASONIC
#define DECODE_LG
#define DECODE_ONKYO
#define DECODE_NEC
#define DECODE_SAMSUNG
#define DECODE_SONY
#define DECODE_RC5
#define DECODE_RC6
#define DECODE_BOSEWAVE
#define DECODE_LEGO_PF
#define DECODE_MAGIQUEST
#define DECODE_WHYNTER
#define DECODE_FAST

#define IR_RECEIVE_PIN PORT_A_WHITE_PIN
#define USE_IRREMOTE_HPP_AS_PLAIN_INCLUDE
#include "IRremote.hpp"
#endif

#ifdef PICOPAD
#include "sound/alarm.c"
#endif

IRDecoder::IRDecoder(LGFX_Sprite *spr) {
	sprite = spr;
}

void IRDecoder::drawSensorData(uint16_t background) {
	sprite->fillRoundRect(10, 35, DISPLAY_WIDTH - 20, 25, 4, COLOR);
	sprite->fillRoundRect(10, 85, DISPLAY_WIDTH - 20, 25, 4, COLOR);
	sprite->fillRoundRect(10, 135, DISPLAY_WIDTH - 20, 25, 4, COLOR);

	sprite->setTextDatum(TL_DATUM);
	sprite->setTextColor(TFT_WHITE, COLOR);
	sprite->setTextFont(2);
	sprite->drawString(F("Protocol"), 20, 40);
	sprite->drawString(F("Address"), 20, 90);
	sprite->drawString(F("Command"), 20, 140);

	sprite->setTextDatum(TC_DATUM);
	sprite->setTextColor(TFT_WHITE, TFT_BLACK);
	sprite->setTextFont(2);
	sprite->drawString(protocol.c_str(), DISPLAY_WIDTH / 2, 65);
	sprite->drawString(address, DISPLAY_WIDTH / 2, 115);
	sprite->drawString(command, DISPLAY_WIDTH / 2, 165);
}

void IRDecoder::open() {

	bool connected = splash(F("IR Decoder"), F("Run"), IRDecoderUnitImg, 100, 80, false);

	if (connected) {
		unit_connected();

#ifndef EMULATOR
		IrReceiver.begin(IR_RECEIVE_PIN);
#ifdef M5STICK
		M5.Speaker.begin();
#endif
#endif
		while (!M5.BtnPWR.wasClicked()) {
			default_actions();

#ifndef EMULATOR
			digitalWrite(M5_LED, HIGH);
			if (IrReceiver.decode()) {
#ifdef M5STICK
				Beep();
#endif
#ifdef PICOPAD
				PLAYSOUND(AlarmSnd);
#endif
				digitalWrite(M5_LED, LOW);
				IrReceiver.printIRResultMinimal(&Serial);
				Serial.println();
				protocol = String(IrReceiver.getProtocolString());
				sprintf(command, "0x%04X", IrReceiver.decodedIRData.command);
				sprintf(address, "0x%04X", IrReceiver.decodedIRData.address);

				delay(100);
				IrReceiver.resume();
			}
#else
			protocol = "Samsung";
			strcpy(command, "0x0002");
			strcpy(address, "0x0707");
#endif

			drawAll(F("IR Decoder"));
			M5.update();
		}
#ifndef EMULATOR
#ifdef M5STICK
		M5.Speaker.end();
#endif
		IrReceiver.end();
		Wire.end();
#endif
	}
}
