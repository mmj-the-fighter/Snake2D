#ifndef _GAME_H_
#define _GAME_H_

#include <SDL3/SDL.h>
#include "../spinach/core/spn_core.h"
#include "../spinach/core/spn_canvas.h"
#include "snake.h"

class Game
{
public:
	Game() {
		snake = nullptr;
	}
	~Game() {
		if (nullptr != snake) {
			delete snake;
		}
	}
	void HandleInput(const SDL_Event* sdlEvent);
	void UpdateAndRender(spn::Canvas* canvas);
	void Init(spn::SpinachCore* canvas);
private:
	Snake* snake;
};


#endif