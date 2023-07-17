#include "Game.h"
#include <cstdlib>

void Game::init() {
	for (int i = 1; i < 5; i++) {
		snake.push_back(Segment(400-i*10, 100, 10, 10, 799, 599, RIGHT));
	}
	fruit.set(400, 300, 10, 10, 799, 599, RIGHT);
}
void Game::restart() {
	score = 0;
	gameOver = false;
	snake.clear();
	init();
	fruit.setPos(rand() % (fruit.ww - fruit.w), rand() % (fruit.wh - fruit.h));
}

void Game::onArrowKeyPressed(Direction d) {
	switch (snake[0].d){
	case LEFT: 
		if (d == LEFT || d == RIGHT)
			return; 
		else 
			break;
	case RIGHT: 
		if (d == LEFT || d == RIGHT) 
			return; 
		else 
			break;
	case UP: 
		if (d == UP || d == DOWN) 
			return; 
		else break;
	case DOWN: 
		if (d == UP || d == DOWN) 
			return; 
		else 
			break;
	}
	
	snake[0].d = d;
}

void Game::tick() {
	int segmentCount = snake.size();
	////Check self collision
	Segment head = snake[0];
	for (int i = 3; i < segmentCount; ++i) {
		const Segment& body = snake[i];
		if (head.checkCollision(body)){
			gameOver = true;
			return;
		}
	}
	//Check collision with fruit
	if (fruit.checkCollision(snake[0])){
		//Relocate fruit
		fruit.setPos(rand() % (fruit.ww - fruit.w), rand() % (fruit.wh - fruit.h));
		//grow snake
		growSnake();
		++score;
	}
	//move segments from tail to just before head
	segmentCount = snake.size();
	for (int i = segmentCount - 1; i >= 1; --i) {
		snake[i].move();
		snake[i].d = snake[i - 1].d;
	}
	//move head
	snake[0].move();
}

void Game::render(swr_sdl_context *ctx) {
	fruit.render(ctx);
	int segmentCount = snake.size();
	for (int i = segmentCount - 1; i >= 0; --i) {
		snake[i].render(ctx);
	}
	
}

void Game::growSnake() {
	int x, y;
	Direction dir = UP;
	int vecx, vecy;
	int segmentCount = snake.size();
	const Segment& s = snake[segmentCount - 1];
	vecx = s.x - snake[segmentCount - 2].x;
	vecy = s.y - snake[segmentCount - 2].y;

	x = s.x;
	y = s.y;

	if (vecx == 0)
	{
		if (vecy > 0)
		{
			dir = UP;
			y += s.h;
		}
		else if (vecy < 0)
		{
			dir = DOWN;
			y -= s.h;
		}
	}
	else if (vecy == 0) {
		if (vecx > 0)
		{
			dir = LEFT;
			x += s.w;
		}
		else if (vecx < 0)
		{
			dir = RIGHT;
			x -= s.w;
		}
	}
	snake.push_back(Segment(x, y, 10, 10, 799, 599, dir));
}

