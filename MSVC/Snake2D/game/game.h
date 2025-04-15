#ifndef _GAME_H_
#define _GAME_H_

#include <SDL3/SDL.h>
#include "../spinach/core/spn_core.h"
#include "../spinach/core/spn_canvas.h"

class Game
{
public:
	void HandleInput(const SDL_Event* sdlEvent);
	void UpdateAndRender(spn::Canvas* canvas);
	void Init(spn::SpinachCore* canvas);
};


#endif