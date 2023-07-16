#include <math.h>
#include <string.h>
#include "swr_rasterizer.h"
#include "swr_pixel.h"

static swr_sdl_context* pcontext = NULL;
static int alpha_blending_flag = 0;

void rasterizer_set_swr_sdl_context(swr_sdl_context* ptr_to_swr_sdl_context)
{
	pcontext = ptr_to_swr_sdl_context;
}

void rasterizer_set_cur_color(int r, int g, int b, int a)
{
	set_swr_color(&pcontext->cur_color, b, g, r, a);
}

void rasterizer_set_clear_color(int r, int g, int b, int a)
{
	set_swr_color(&pcontext->clear_color, b, g, r, a);
}

void enable_alpha_blending(int flag)
{
	alpha_blending_flag = flag;
}

void rasterizer_put_pixel(int x, int y)
{
	unsigned char* loc = 
		pcontext->screen_texture_pixels + 
		pcontext->screen_texture_pitch * y + 
		pcontext->screen_texture_channels * x;
	*loc++ = pcontext->cur_color.b;
	*loc++ = pcontext->cur_color.g;
	*loc++ = pcontext->cur_color.r;
	*loc = pcontext->cur_color.a;
}

void rasterizer_put_pixel2(int x, int y, swr_color *pc)
{
	unsigned char* loc =
		pcontext->screen_texture_pixels +
		pcontext->screen_texture_pitch * y +
		pcontext->screen_texture_channels * x;
	*loc++ = pc->b;
	*loc++ = pc->g;
	*loc++ = pc->r;
	*loc = pc->a;
}

void rasterizer_draw_line_dda(int x0, int y0, int x1, int y1)
{
	float x, y, xincr, yincr;
	int steps;
	int dx = x1 - x0;
	int dy = y1 - y0;

	if (abs(dx) > abs(dy)) {
		steps = abs(dx);
		xincr = dx / (float)steps;
		yincr = dy / (float)steps;
	}
	else
	{
		steps = abs(dy);
		xincr = dx / (float)steps;
		yincr = dy / (float)steps;
	}
	x = (float)x0;
	y = (float)y0;
	rasterizer_put_pixel((int)(x + 0.5), (int)(y + 0.5));
	for (int i = 0; i < steps; i++)
	{
		x += xincr;
		y += yincr;
		rasterizer_put_pixel((int)(x + 0.5), (int)(y + 0.5));
	}
}

void rasterizer_draw_line_bres(int x0, int y0, int x1, int y1)
{
	int x, y, dx, dy, adx, ady, s1, s2, p, i;

	dx = x1 - x0;
	dy = y1 - y0;

	if (dx > 0) {
		s1 = 1;
	}
	else if (dx < 0) {
		s1 = -1;
	}
	else {
		s1 = 0;
	}

	if (dy > 0) {
		s2 = 1;
	}
	else if (dy < 0) {
		s2 = -1;
	}
	else {
		s2 = 0;
	}
		
	x = x0;
	y = y0;
	adx = abs(dx);
	ady = abs(dy);

	if (ady > adx)
	{
		p = 2 * adx - ady;
		for (i = 0; i < ady; i++)
		{
			rasterizer_put_pixel(x, y);
			while (p >= 0)
			{
				p = p - 2 * ady;
				x += s1;
			}
			p = p + 2 * adx;
			y += s2;
		}
		rasterizer_put_pixel(x1, y1);
	}
	else
	{
		p = 2 * ady - adx;
		for (i = 0; i < adx; i++)
		{
			rasterizer_put_pixel(x, y);
			while (p >= 0)
			{
				p = p - 2 * adx;
				y += s2;
			}
			p = p + 2 * ady;
			x += s1;
		}
		rasterizer_put_pixel(x1, y1);
	}
}


void rasterizer_fill_rect(int x0, int y0, int x1, int y1)
{
	unsigned char* pixels = pcontext->screen_texture_pixels;
	unsigned char* loc;
	int pitch = pcontext->screen_texture_pitch;
	int channels = pcontext->screen_texture_channels;
	int x, y;

	
	for (y = y0; y <= y1; ++y)
	{
		for (x = x0; x <= x1; ++x)
		{
			loc = pixels + pitch * y + x * channels;
			*loc++ = pcontext->cur_color.b;
			*loc++ = pcontext->cur_color.g;
			*loc++ = pcontext->cur_color.r;
			*loc = pcontext->cur_color.a;
		}
	}
}

void rasterizer_draw_rect(int left, int top, int right, int bottom)
{
	rasterizer_draw_line_dda(left, top, right, top);
	rasterizer_draw_line_dda(right, top, right, bottom);
	rasterizer_draw_line_dda(right, bottom, left, bottom);
	rasterizer_draw_line_dda(left, bottom, left, top);
}

void rasterizer_copy_pixels(
	int dst_x_start,
	int dst_y_start,
	int src_width,
	int src_height,
	unsigned char* src_pixels)
{
	unsigned char *dst_pixels = pcontext->screen_texture_pixels;
	unsigned char *dst_loc, *src_loc;
	int src_pitch = src_width * pcontext->screen_texture_channels;
	int dst_width = pcontext->screen_texture_pixels_wide;
	int dst_height = pcontext->screen_texture_pixels_high;
	int dst_pitch = pcontext->screen_texture_pitch;
	int channels = pcontext->screen_texture_channels;
	int x, y;
	int offset_x;
	int offset_y;
	double src_alpha;
	double one_minus_src_alpha;
	double inv_255 = 1.0 / 255.0;
	unsigned char src_alpha_uchar;

	/* sanity check dst_start coordinates */
	if (
		(dst_x_start < 0 || dst_x_start >= dst_width) ||
		(dst_y_start < 0 || dst_y_start >= dst_height)
		)
	{
		return;
	}

	/* clip */
	offset_x = (dst_x_start + src_width) - dst_width;
	offset_y = (dst_y_start + src_height) - dst_height;
	if (offset_x > 0) {
		src_width -= offset_x;
	}
	if (offset_y > 0) {
		src_height -= offset_y;
	}

	/* iterate through source rectangle */
	/* copy to destination */
	
	for (y = 0; y < src_height; ++y)
	{
		for (x = 0; x < src_width; ++x)
		{
			src_loc = src_pixels + src_pitch * y + x * channels;
				dst_loc = dst_pixels + 
					dst_pitch * (dst_y_start + y) + 
					(dst_x_start + x) * channels;
				
			if (alpha_blending_flag) {
				/* blend */
				/* SourceColorAlpha * SourceColorBGR + (1 - SourceColorAlpha) * DestinationColorBGR */
				src_alpha_uchar = *(src_loc + 3);
				src_alpha = src_alpha_uchar * inv_255;
				one_minus_src_alpha = 1.0 - src_alpha;
				*dst_loc++ = src_alpha**src_loc++ + *dst_loc * one_minus_src_alpha;
				*dst_loc++ = src_alpha**src_loc++ + *dst_loc * one_minus_src_alpha;
				*dst_loc++ = src_alpha**src_loc++ + *dst_loc * one_minus_src_alpha;
				*dst_loc = src_alpha_uchar;
			}
			else
			{
				*dst_loc++ = *src_loc++;
				*dst_loc++ = *src_loc++;
				*dst_loc++ = *src_loc++;
				*dst_loc = *src_loc;
			}
		}
	}
}

void rasterizer_copy_pixels_subimage(
	int dst_x_start,
	int dst_y_start,
	int src_x_start,
	int src_y_start,
	int src_width,
	int src_height,
	int src_total_width,
	int src_total_height,
	unsigned char* src_pixels)
{
	unsigned char *dst_pixels = pcontext->screen_texture_pixels;
	unsigned char *dst_loc, *src_loc;
	int src_pitch = src_total_width * pcontext->screen_texture_channels;
	int dst_total_width = pcontext->screen_texture_pixels_wide;
	int dst_total_height = pcontext->screen_texture_pixels_high;
	int dst_pitch = pcontext->screen_texture_pitch;
	int channels = pcontext->screen_texture_channels;
	int src_x, src_y, dst_x, dst_y;
	int offset_x;
	int offset_y;
	int x, y;
	double src_alpha;
	double one_minus_src_alpha;
	double inv_255 = 1.0 / 255.0;
	unsigned char src_alpha_uchar;


	/* sanity check dst_start and src_start coordinates */
	if (
		(dst_x_start < 0 || dst_x_start >= dst_total_width) ||
		(dst_y_start < 0 || dst_y_start >= dst_total_height) ||
		(src_x_start < 0 || src_x_start >= src_total_width) ||
		(src_y_start < 0 || src_y_start >= src_total_height)
		)
	{
		return;
	}

	/* clip at source */
	offset_x = (src_x_start + src_width) - src_total_width;
	offset_y = (src_y_start + src_height) - src_total_height;
	if (offset_x > 0) {
		src_width -= offset_x;
	}
	if (offset_y > 0) {
		src_height -= offset_y;
	}

	/* clip at destination */
	offset_x = (dst_x_start + src_width) - dst_total_width;
	offset_y = (dst_y_start + src_height) - dst_total_height;
	if (offset_x > 0) {
		src_width -= offset_x;
	}
	if (offset_y > 0) {
		src_height -= offset_y;
	}

	/* iterate through source rectangle */
	/* copy to destination */
	for (src_y = src_y_start, y = 0; src_y < (src_y_start + src_height); ++src_y, ++y)
	{
		for (src_x = src_x_start, x = 0; src_x < (src_x_start + src_width); ++src_x, ++x)
		{
			dst_x = dst_x_start + x;
			dst_y = dst_y_start + y;
			src_loc = src_pixels + src_pitch * src_y + src_x * channels;
			dst_loc = dst_pixels + dst_pitch * dst_y + dst_x * channels;
			if (alpha_blending_flag) {
				/* blend */
				/* SourceColorAlpha * SourceColorBGR + (1 - SourceColorAlpha) * DestinationColorBGR */
				src_alpha_uchar = *(src_loc + 3);
				src_alpha = src_alpha_uchar * inv_255;
				one_minus_src_alpha = 1.0 - src_alpha;
				*dst_loc++ = src_alpha**src_loc++ + *dst_loc * one_minus_src_alpha;
				*dst_loc++ = src_alpha**src_loc++ + *dst_loc * one_minus_src_alpha;
				*dst_loc++ = src_alpha**src_loc++ + *dst_loc * one_minus_src_alpha;
				*dst_loc = src_alpha_uchar;
			}
			else {
				*dst_loc++ = *src_loc++;
				*dst_loc++ = *src_loc++;
				*dst_loc++ = *src_loc++;
				*dst_loc = *src_loc;
			}
		}
	}
}


void rasterizer_copy_pixels_subimage_chromakey(
	int dst_x_start,
	int dst_y_start,
	int src_x_start,
	int src_y_start,
	int src_width,
	int src_height,
	int src_total_width,
	int src_total_height,
	swr_color* chroma_key,
	unsigned char* src_pixels)
{
	unsigned char *dst_pixels = pcontext->screen_texture_pixels;
	unsigned char *dst_loc, *src_loc;
	int src_pitch = src_total_width * pcontext->screen_texture_channels;
	int dst_total_width = pcontext->screen_texture_pixels_wide;
	int dst_total_height = pcontext->screen_texture_pixels_high;
	int dst_pitch = pcontext->screen_texture_pitch;
	int channels = pcontext->screen_texture_channels;
	int src_x, src_y,dst_x,dst_y;
	int offset_x;
	int offset_y;
	int x, y;


	/* sanity check dst_start and src_start coordinates */
	if (
		(dst_x_start < 0 || dst_x_start >= dst_total_width) ||
		(dst_y_start < 0 || dst_y_start >= dst_total_height) ||
		(src_x_start < 0 || src_x_start >= src_total_width) ||
		(src_y_start < 0 || src_y_start >= src_total_height)
		)
	{
		return;
	}

	/* clip at source */
	offset_x = (src_x_start + src_width) - src_total_width;
	offset_y = (src_y_start + src_height) - src_total_height;
	if (offset_x > 0) {
		src_width -= offset_x;
	}

	if (offset_y > 0) {
		src_height -= offset_y;
	}

	/* clip at destination */
	offset_x = (dst_x_start + src_width) - dst_total_width;
	offset_y = (dst_y_start + src_height) - dst_total_height;
	if (offset_x > 0) {
		src_width -= offset_x;
	}

	if (offset_y > 0) {
		src_height -= offset_y;
	}


	/* iterate through source rectangle */
	/* copy to destination */
	for (src_y = src_y_start, y = 0; src_y < (src_y_start + src_height); ++src_y, ++y)
	{
		for (src_x = src_x_start, x = 0; src_x < (src_x_start + src_width); ++src_x, ++x)
		{
			dst_x = dst_x_start + x;
			dst_y = dst_y_start + y;
			src_loc = src_pixels + src_pitch * src_y + src_x * channels;
			dst_loc = dst_pixels + dst_pitch * dst_y + dst_x * channels;
			if (chroma_key->b == *src_loc &&
				chroma_key->g == *(src_loc + 1) &&
				chroma_key->r == *(src_loc + 2)) 
			{
				dst_loc += 4;
			}
			else {
				*dst_loc++ = *src_loc++;
				*dst_loc++ = *src_loc++;
				*dst_loc++ = *src_loc++;
				*dst_loc = *src_loc;
			}
		}
	}
}

void rasterizer_copy_pixels_color_replace_subimage_chromakey(
	int dst_x_start,
	int dst_y_start,
	int src_x_start,
	int src_y_start,
	int src_width,
	int src_height,
	int src_total_width,
	int src_total_height,
	swr_color *chroma_key,
	swr_color *newcolor,
	unsigned char* src_pixels)
{
	unsigned char *dst_pixels = pcontext->screen_texture_pixels;
	unsigned char *dst_loc, *src_loc;
	int src_pitch = src_total_width * pcontext->screen_texture_channels;
	int dst_total_width = pcontext->screen_texture_pixels_wide;
	int dst_total_height = pcontext->screen_texture_pixels_high;
	int dst_pitch = pcontext->screen_texture_pitch;
	int channels = pcontext->screen_texture_channels;
	int src_x, src_y, dst_x, dst_y;
	int offset_x;
	int offset_y;
	int x, y;


	/* sanity check dst_start and src_start coordinates */
	if (
		(dst_x_start < 0 || dst_x_start >= dst_total_width) ||
		(dst_y_start < 0 || dst_y_start >= dst_total_height) ||
		(src_x_start < 0 || src_x_start >= src_total_width) ||
		(src_y_start < 0 || src_y_start >= src_total_height)
		)
	{
		return;
	}

	/* clip at source */
	offset_x = (src_x_start + src_width) - src_total_width;
	offset_y = (src_y_start + src_height) - src_total_height;
	if (offset_x > 0) {
		src_width -= offset_x;
	}

	if (offset_y > 0) {
		src_height -= offset_y;
	}

	/* clip at destination */
	offset_x = (dst_x_start + src_width) - dst_total_width;
	offset_y = (dst_y_start + src_height) - dst_total_height;
	if (offset_x > 0) {
		src_width -= offset_x;
	}

	if (offset_y > 0) {
		src_height -= offset_y;
	}


	/* iterate through source rectangle */
	/* copy to destination */
	for (src_y = src_y_start, y = 0; src_y < (src_y_start + src_height); ++src_y, ++y)
	{
		for (src_x = src_x_start, x = 0; src_x < (src_x_start + src_width); ++src_x, ++x)
		{
			dst_x = dst_x_start + x;
			dst_y = dst_y_start + y;
			src_loc = src_pixels + src_pitch * src_y + src_x * channels;
			dst_loc = dst_pixels + dst_pitch * dst_y + dst_x * channels;
			if (chroma_key->b == *src_loc &&
				chroma_key->g == *(src_loc + 1) &&
				chroma_key->r == *(src_loc + 2))
			{
				dst_loc += 4;
			}
			else {
				*dst_loc++ = newcolor->b; 
				*dst_loc++ = newcolor->g;
				*dst_loc++ = newcolor->b;
				*dst_loc = newcolor->a;
				src_loc += 4;
			}
		}
	}
}


void rasterizer_draw_text(swr_rfont *f, int x, int y, char* text)
{
	unsigned char c;
	int i;
	int maxCols = maxCols = f->font_image_width / f->font_cell_width;
	unsigned char baseChar = f->font_base_char;
	unsigned char charDiff;
	int row, col;
	swr_color chroma_key = { 255, 0, 255, 255 };
	i = 0;
	while ((c = text[i]) != '\0')
	{
		charDiff = c - baseChar;
		row = charDiff / maxCols;
		col = charDiff - (row*maxCols);
		rasterizer_copy_pixels_subimage_chromakey
			(x, y, col*f->font_cell_width, row*f->font_cell_height, 
			f->font_cell_width, f->font_cell_height, f->font_image_width, 
			f->font_image_height, &chroma_key, f->font_image_pixels);
		x += f->font_width[c];
		++i;
	}
}



void rasterizer_draw_text_with_color(swr_rfont *f, swr_color *color, int x, int y, char* text)
{
	unsigned char c;
	int i;
	int maxCols = maxCols = f->font_image_width / f->font_cell_width;
	unsigned char baseChar = f->font_base_char;
	unsigned char charDiff;
	int row, col;
	swr_color chroma_key = { 255, 0, 255, 255 };
	i = 0;
	while ((c = text[i]) != '\0')
	{
		charDiff = c - baseChar;
		row = charDiff / maxCols;
		col = charDiff - (row*maxCols);
		rasterizer_copy_pixels_color_replace_subimage_chromakey
			(x, y, col*f->font_cell_width, row*f->font_cell_height,
			f->font_cell_width, f->font_cell_height, f->font_image_width,
			f->font_image_height, &chroma_key, color, f->font_image_pixels);
		x += f->font_width[c];
		++i;
	}
}
void rasterizer_clear()
{
	unsigned char* loc = pcontext->screen_texture_pixels;
	int n = pcontext->screen_texture_pixels_wide * 
		pcontext->screen_texture_pixels_high;
	int i;

	unsigned char b = pcontext->clear_color.b, 
		g = pcontext->clear_color.g, 
		r = pcontext->clear_color.r, 
		a = pcontext->clear_color.a;

	for (i = 0; i < n; ++i) 
	{
		*loc++ = b;
		*loc++ = g;
		*loc++ = r;
		*loc++ = a;
	}
}
