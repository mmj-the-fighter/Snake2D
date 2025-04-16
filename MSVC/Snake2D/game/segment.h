#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include "../spinach/core/spn_core.h"
#include "../spinach/core/spn_canvas.h"

enum class MoveDirection 
{
	LEFT, RIGHT, UP, DOWN
};

class Segment
{
public:
	Segment() {};
	Segment(int xpos, int ypos, 
		int width, int height, 
		int worldWidth, int worldHeight, 
		MoveDirection direction, int segmentType);
	void Set(int xpos, int ypos, 
		int width, int height, 
		int worldWidth, int worldHeight, 
		MoveDirection direction, int segmentType);
	void SetPos(int xpos, int ypos);
	void Move();
	void Render(spn::Canvas *canvas);
	bool CheckCollision(const Segment& segment);
public:
	int x;
	int y;
	MoveDirection moveDir;
	int w;
	int h;
	int ww;
	int wh;
	int type;
private:
	bool CheckCollision(
		int left1, int top1, 
		int right1, int bottom1,
		int left2, int top2, 
		int right2, int bottom2
	);
};


#endif