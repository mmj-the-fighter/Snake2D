#include <stdio.h>
#include <string.h>
#include <math.h>
#include "swr_sdl_window.h"
#include "swr_rasterizer.h"
#include "swr_image.h"
#include "swr_rfont.h"
#include "Segment.h"
#include "Game.h"

swr_sdl_context* ctx;
swr_rfont* font;
swr_color textcolor = { 0, 0, 255, 255 };
int worldWidth = 800;
int worldHeight = 600;
Game game;
char buf[256];

void display(swr_sdl_context *ctx)
{
	rasterizer_clear();
	game.tick();
	game.render(ctx);
	if (game.gameOver){
		rasterizer_draw_text(font, 800/2, 600/2, "Game Over!");
	}
	sprintf(buf, "Score: %d", game.score);
	rasterizer_draw_text(font, 50, 50, buf);
	
}

void capture_screen()
{
	write_bmp2(NULL, ctx->screen_texture_pixels,
		ctx->screen_texture_pixels_wide,
		ctx->screen_texture_pixels_high);
}

int input(SDL_Event* e)
{
	if (e->type == SDL_KEYDOWN) {
		switch (e->key.keysym.scancode)
		{
		case SDL_SCANCODE_ESCAPE:
			return 1;
		case SDL_SCANCODE_F12:
			capture_screen();
			break;
		case SDL_SCANCODE_LEFT:
			game.onArrowKeyPressed(LEFT);
			break;
		case SDL_SCANCODE_RIGHT:
			game.onArrowKeyPressed(RIGHT);
			break;
		case SDL_SCANCODE_UP:
			game.onArrowKeyPressed(UP);
			break;
		case SDL_SCANCODE_DOWN:
			game.onArrowKeyPressed(DOWN);
			break;
		}
	}
	return 0;
}



int main(int argc, char **argv)
{
	font = create_font("glyphs.ppm", "glyphs.csv");
	swr_sdl_create_context(worldWidth, worldHeight);
	swr_sdl_set_input_handler(input);
	swr_sdl_set_display_handler(display);
	ctx = swr_sdl_get_context();
	rasterizer_set_swr_sdl_context(ctx);
	rasterizer_set_cur_color(255, 255, 255, ctx->opaque_alpha_value);
	rasterizer_set_clear_color(0, 0, 128, ctx->transparent_alpha_value);
	rasterizer_clear();
	swr_sdl_main_loop();

	destroy_font(font);
	swr_sdl_destroy_context();
	return 0;
}
