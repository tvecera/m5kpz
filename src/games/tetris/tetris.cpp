/**
* MIT License
*
* Copyright (c) 2024 Tomas Vecera, tomas@vecera.dev
*
* This software is based on the Volos Tetris:
*   Repository: https://github.com/VolosR/M5ScickPlusTetris/blob/main/
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
#include "tetris.h"
#include "tet.h"

#ifdef M5STICK
#include "joystick.h"
#endif

Tetris::Tetris(LGFX_Sprite *spr) {
	sprite = spr;
}

bool Tetris::initSensor() {
#ifdef M5STICK
	auto *instance = static_cast<Joystick *>(pJoystick);
	return instance->begin();
#else
	return true;
#endif
}

void Tetris::open() {
#ifdef M5STICK
	auto *joystick = new Joystick();
	pJoystick = joystick;
#endif
	bool connected = splash(F("Tetris"), F("Run"), TetrisUnitImg, 100, 100, true);

	if (connected) {
		pDisplay = sprite->getParent();
		pDisplay->setSwapBytes(true);
#ifdef M5STICK
		pDisplay->setRotation(2);
#endif

		pDisplay->pushImage(START_X, 0, 135, 240, tet);
		delay(3000);
		pDisplay->fillScreen(TFT_BLACK);
		pDisplay->drawLine(START_X + 11, 19, START_X + 122, 19, TFT_GREY);
		pDisplay->drawLine(START_X + 11, 19, START_X + 11, 240, TFT_GREY);
		pDisplay->drawLine(START_X + 122, 19, START_X + 122, 240, TFT_GREY);
		pDisplay->setTextFont(1);
		pDisplay->drawString(("SCORE:" + String(score)).c_str(), START_X + 14, 8);
		pDisplay->drawString(("LVL:" + String(lvl)).c_str(), START_X + 88, 8);

		//----------------------------// Make Block ----------------------------
		make_block(0, TFT_BLACK);        // Type No, Color
		make_block(1, 0x00F0);       // DDDD     RED
		make_block(2, 0xFBE4);       // DD,DD    PUPLE
		make_block(3, 0xFF00);       // D__,DDD  BLUE
		make_block(4, 0xFF87);       // DD_,_DD  GREEN
		make_block(5, 0x87FF);       // __D,DDD  YELLO
		make_block(6, 0xF00F);       // _DD,DD_  LIGHT GREEN
		make_block(7, 0xF8FC);       // _D_,DDD  PINK
		//----------------------------------------------------------------------

		PutStartPos();
		for (int i = 0; i < 4; ++i)
			screen[pos.X + block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = block.color;
		Draw();
		M5.update();

		while (!M5.BtnPWR.wasClicked()) {
			default_actions();
#ifdef M5STICK
			xj = joystick->getX();
			bj = joystick->getB();
#endif
#ifdef EMULATOR
			if (M5.BtnC.wasClicked() || M5.BtnA.wasClicked() || M5.BtnB.wasClicked()) {
				if (M5.BtnC.wasClicked()) { xj = -120; }
				if (M5.BtnA.wasClicked()) { xj = 120; }
				if (M5.BtnB.wasClicked()) { bj = 0; } else { bj = 1; }
			} else {
				xj = 0;
				bj = 1;
			}
#endif
#ifdef PICOPAD
			if (M5.BtnLeft.wasClicked() || M5.BtnRight.wasClicked() || M5.BtnA.wasClicked()) {
				if (M5.BtnRight.wasClicked()) { xj = -120; }
				if (M5.BtnLeft.wasClicked()) { xj = 120; }
				if (M5.BtnA.wasClicked()) { bj = 0; } else { bj = 1; }
			} else {
				xj = 0;
				bj = 1;
			}
#endif

			if (gameover) {
				if (xj > 110) {
					for (int j = 0; j < Height; ++j) {
						for (int i = 0; i < Width; ++i) {
							screen[i][j] = 0;
						}
					}
					gameover = false;
					score = 0;
					game_speed = 20;
					lvl = 1;
					PutStartPos();
					for (int i = 0; i < 4; ++i) {
						screen[pos.X + block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = block.color;
					}
					pDisplay->setTextFont(1);
					pDisplay->drawString(("SCORE:" + String(score)).c_str(), START_X + 14, 8);
					pDisplay->drawString(("LVL:" + String(lvl)).c_str(), START_X + 88, 8);
					Draw();
				}
			}

			if (!gameover) {
				Point next_pos = {};
				int next_rot = rot;
				GetNextPosRot(&next_pos, &next_rot);
				ReviseScreen(next_pos, next_rot);
				delay(game_speed);
			}
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

// Draw 120x240 in the center
void Tetris::Draw() {
	for (int i = 0; i < Width; ++i) {
		for (int j = 0; j < Height; ++j) {
			for (int k = 0; k < Length; ++k) {
				for (int l = 0; l < Length; ++l) {
					backBuffer[j * Length + l][i * Length + k] = BlockImage[screen[i][j]][k][l];
				}
			}
		}
	}
	pDisplay->pushImage(START_X + 12, 20, 110, 220, (uint16_t *) backBuffer);
}

void Tetris::PutStartPos() {
	game_speed = 20;
	pos.X = 4;
	pos.Y = 1;
	block = blocks[RANDOM2(7)];
	rot = RANDOM2(block.numRotate);
}

bool Tetris::GetSquares(Block blockValue, Point posValue, int rotValue, Point *squares) {
	bool overlap = false;
	for (int i = 0; i < 4; ++i) {
		Point pValue = {};
		pValue.X = posValue.X + blockValue.square[rotValue][i].X;
		pValue.Y = posValue.Y + blockValue.square[rotValue][i].Y;
		overlap |= pValue.X < 0 || pValue.X >= Width || pValue.Y < 0 || pValue.Y >= Height
							 || screen[pValue.X][pValue.Y] != 0;
		squares[i] = pValue;
	}
	return !overlap;
}

void Tetris::GameOver() {
	for (int i = 0; i < Width; ++i) {
		for (int j = 0; j < Height; ++j) {
			if (screen[i][j] != 0) {
				screen[i][j] = 4;
			}
		}
	}
	gameover = true;
}

void Tetris::ClearKeys() {
	but_A = false;
	but_LEFT = false;
	but_RIGHT = false;
	xj = 0;
	bj = 0;
}

bool Tetris::KeyPadLoop() {
	if (xj > 110) {
		if (pom == 0) {
			pom = 1;
			ClearKeys();
			but_LEFT = true;
			return true;
		}
	} else { pom = 0; }

	if (xj < -110) {
		if (pom2 == 0) {
			pom2 = 1;
			ClearKeys();
			but_RIGHT = true;
			return true;
		}
	} else { pom2 = 0; }

#ifdef M5STICK
	if (M5.BtnA.wasClicked()) {
		game_speed = 1;
	}
#endif
#ifdef PICOPAD
	if (M5.BtnDown.wasClicked()) {
		game_speed = 1;
	}
#endif

	if (bj == 0) {
		if (pom3 == 0) {
			pom3 = 1;
			ClearKeys();
			but_A = true;
			return true;
		}
	} else { pom3 = 0; }

	return false;
}

void Tetris::GetNextPosRot(Point *pnext_pos, int *pnext_rot) {
	KeyPadLoop();

	if (but_LEFT) started = true;
	if (!started) return;

	pnext_pos->X = pos.X;
	pnext_pos->Y = pos.Y;
	if ((fall_cnt = (fall_cnt + 1) % 10) == 0) pnext_pos->Y += 1;
	else if (but_LEFT) {
		but_LEFT = false;
		pnext_pos->X -= 1;
	} else if (but_RIGHT) {
		but_RIGHT = false;
		pnext_pos->X += 1;
	} else if (but_A) {
		but_A = false;
		*pnext_rot = (*pnext_rot + block.numRotate - 1) % block.numRotate;
	}
}

void Tetris::DeleteLine() {
	for (int j = 0; j < Height; ++j) {
		bool Delete = true;
		for (int i = 0; i < Width; ++i) {
			if (screen[i][j] == 0) Delete = false;
		}
		if (Delete) {
			score++;
			if (score % 5 == 0) {
				lvl++;
				game_speed = game_speed - 4;
				pDisplay->setTextFont(1);
				pDisplay->drawString(("LVL:" + String(lvl)).c_str(), START_X + 88, 8);
			}
			pDisplay->setTextFont(1);
			pDisplay->drawString(("SCORE:" + String(score)).c_str(), START_X + 14, 8);
			for (int k = j; k >= 1; --k) {
				for (int i = 0; i < Width; ++i) {
					screen[i][k] = screen[i][k - 1];
				}
			}
		}
	}
}

void Tetris::ReviseScreen(Point next_pos, int next_rot) {
	if (!started) return;
	Point next_squares[4];
	for (int i = 0; i < 4; ++i) {
		screen[pos.X + block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = 0;
	}
	if (GetSquares(block, next_pos, next_rot, next_squares)) {
		for (auto &next_square: next_squares) {
			screen[next_square.X][next_square.Y] = block.color;
		}
		pos = next_pos;
		rot = next_rot;
	} else {
		for (int i = 0; i < 4; ++i) {
			screen[pos.X + block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = block.color;
		}
		if (next_pos.Y == pos.Y + 1) {
			DeleteLine();
			PutStartPos();
			if (!GetSquares(block, pos, rot, next_squares)) {
				for (int i = 0; i < 4; ++i) {
					screen[pos.X + block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = block.color;
				}
				GameOver();
			}
		}
	}
	Draw();
}

void Tetris::make_block(int n, uint16_t color) {
	// Make Block color
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 12; j++) {
			// Block color
			BlockImage[n][i][j] = color;
			// TFT_BLACK Line
			if (i == 0 || j == 0) BlockImage[n][i][j] = 0;
		}
	}
}
