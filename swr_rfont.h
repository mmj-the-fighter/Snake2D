#ifndef _SWR_RFONT_H_
#define _SWR_RFONT_H_

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct _swr_rfont
	{
		unsigned char *font_image_pixels;
		int font_image_width;
		int font_image_height;
		int font_cell_width;
		int font_cell_height;
		int font_height;
		int font_width[256];
		unsigned char font_base_char;
	}swr_rfont;

	
	swr_rfont * create_font(const char* fontimagefilename, const char* fontpropertiescsvfilename);
	void destroy_font(swr_rfont* font);

	void print_font_info(swr_rfont* font);
	



#ifdef __cplusplus
}
#endif

#endif