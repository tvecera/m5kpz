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
#ifdef EMULATOR

#include <M5GFX.h>

using namespace lgfx;

void setup();

void loop();

int run(int(*fn)(const bool *)) {
	if (0 != Panel_sdl::setup()) { return 1; }

	bool running = true;
	auto *thread = SDL_CreateThread((SDL_ThreadFunction) fn, "fn", &running);

	while (0 == Panel_sdl::loop()) {}

	running = false;
	SDL_DetachThread(thread);

	return Panel_sdl::close();
}

__attribute__((weak))
int user_func(const bool *running) {
	setup();
	do {
		loop();
	} while (*running);
	return 0;
}

int main(int, char **) {
	return run(user_func);
}

#endif