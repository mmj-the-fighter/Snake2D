#include <iostream>
#include "game.h"


void Game::HandleInput(const SDL_Event* sdlEvent) {

	switch (sdlEvent->type) {
		case SDL_EVENT_KEY_DOWN:
			switch (sdlEvent->key.key) {
				case SDLK_LEFT:
					std::cout << "Left\n";
					break;
				case SDLK_RIGHT:
					std::cout << "Right\n";
					break;
				case SDLK_UP:
					std::cout << "Up\n";
					break;
				case SDLK_DOWN:
					std::cout << "Down\n";
				break;
			}
		break;
	}
}

void Game::UpdateAndRender(spn::Canvas* canvas) {
	canvas->Clear();
}

void Game::Init(spn::SpinachCore* core) {
	spn::Canvas* canvas = core->GetCanvas();
	canvas->SetClearColor(0, 0, 128);
	canvas->SetPrimaryColor(255, 255, 0);
	core->SetTargetFramesPerSecond(30);
}
