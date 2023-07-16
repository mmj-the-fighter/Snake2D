#ifndef _GAME_H_
#define _GAME_H_
#include <vector>
#include "Segment.h" 


class Game
{
public:
	Game(){ score = 0; gameOver = false; init(); }
	void init();
	void onArrowKeyPressed(Direction d);
	void spawnFruit();
	void growSnake();
	void tick();
	void render(swr_sdl_context *ctx);
private:
	std::vector<Segment> snake;
	Segment fruit;
public:
	bool gameOver;
	int score;
};

#endif