#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include "types.h"

// palette indexes
#define BLACK  0
#define GREY   1
#define WHITE  2
#define RED    3
#define ORANGE 4
#define YELLOW 5
#define GREEN  6
#define BLUE   7
#define INDIGO 8
#define VIOLET 9

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

void draw_rect(u8 *fb_base, int x, int y, int width, int height, u8 color);
void draw_char(u8 *fb_base, char c, int x, int y);
void draw_string(u8 *fb_base, const char *s, int x, int y);
u8* get_fb();

#endif /*_FRAMEBUFFER_H */
