#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include "swr_sdl_context.h" 

enum Direction{
	LEFT, RIGHT, UP, DOWN
};

class Segment
{
public :
	Segment(){};
	Segment(int xpos, int ypos, int width, int height, int worldWidth, int worldHeight, Direction direction);
	void set(int xpos, int ypos, int width, int height, int worldWidth, int worldHeight, Direction direction);
	void setPos(int xpos, int ypos);
	void move();
	void render(swr_sdl_context *ctx);
	bool checkCollision(const Segment& segment);
public:
	int x;
	int y;
	Direction d;
	int w;
	int h;
	int ww;
	int wh;
private:
	bool checkCollision(
		int left1, int top1, int right1, int bottom1,
		int left2, int top2, int right2, int bottom2
		);

};

#endif