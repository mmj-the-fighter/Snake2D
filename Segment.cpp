#include "Segment.h"
#include "swr_rasterizer.h"

Segment::Segment(int xpos, int ypos, int width, int height, int worldWidth, int worldHeight, Direction direction) :
x(xpos), y(ypos), w(width), h(height), ww(worldWidth), wh(worldHeight), d(direction){}

void Segment::set(int xpos, int ypos, int width, int height, int worldWidth, int worldHeight, Direction direction)
{
	x = xpos;
	y = ypos;
	w = width;
	h = height;
	ww = worldWidth;
	wh = worldHeight;
}

void Segment::setPos(int xpos, int ypos)
{
	x = xpos;
	y = ypos;
}

void Segment::move(){
	switch (d)
	{
	case UP:
		y -= h;
		if (y < 0){
			y = wh;
		}
		break;
	case DOWN:
		y += h;
		y %= wh;
		break;
	case RIGHT:
		x += w;
		x %= ww;
		break;
	case LEFT:
		x -= w;
		if (x < 0){
			x = ww;
		}
		break;
	}
}

void Segment::render(swr_sdl_context *ctx){
	int x1 = x + w;
	int y1 = y + h;

	if (x1 >= ctx->screen_texture_pixels_wide){
		x1 = ctx->screen_texture_pixels_wide - 1;
	}
	if (y1 >= ctx->screen_texture_pixels_high){
		y1 = ctx->screen_texture_pixels_high - 1;
	}
	rasterizer_fill_rect(x, y, x1, y1);
}

bool Segment::checkCollision(
	int left1, int top1, int right1, int bottom1,
	int left2, int top2, int right2, int bottom2
	)
{
	int  x1, x2;
	// If it is outside our Vertical Height limit simply neglect it!
	if (bottom2 < top1 || top2 > bottom1){
		return false;
	}
	else
	{
		x1 = (left2  > left1) ? left2 : left1;
		x2 = (right2 < right1) ? right2 : right1;
		if (x1 > x2)
			return false;
		return true;
	}
}

bool Segment::checkCollision(const Segment& s){
	return checkCollision(
		s.x,
		s.y,
		s.x + s.w,
		s.y + s.h,
		x,
		y,
		x + w,
		y + h
		);
}

