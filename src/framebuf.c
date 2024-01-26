#include "framebuf.h"
#include "types.h"
#include "mailbox.h"
#include "font.h"


// 16-byte aligned mailbox buffer
static u32 mailbox_init_fb[42] __attribute__((aligned(16))) = {
    42 * 4,        // total buffer size in bytes
    RPI_FIRMWARE_STATUS_REQUEST,
    RPI_FIRMWARE_FRAMEBUFFER_SET_PHYSICAL_WIDTH_HEIGHT,
    8,
    0,
    SCREEN_WIDTH,   // request: width              response: width or 0 if unsupported
    SCREEN_HEIGHT,  // request: height             response: height or 0 if unsupported
    RPI_FIRMWARE_FRAMEBUFFER_SET_VIRTUAL_WIDTH_HEIGHT,
    8,
    0,
    SCREEN_WIDTH,   // request: width              response: width or 0 if unsupported
    SCREEN_HEIGHT,  // request: height             response: height or 0 if unsupported
    RPI_FIRMWARE_FRAMEBUFFER_SET_VIRTUAL_OFFSET,
    8,
    0,
    0,              // request: x offset           response: x offset or 0 if unsupported
    0,              // request: y offset           response: y offset or 0 if unsupported
    RPI_FIRMWARE_FRAMEBUFFER_SET_DEPTH,
    4,
    0,
    8,              // request: bits per pixel     response: bits per pixel or 0 if unsupported
    RPI_FIRMWARE_FRAMEBUFFER_SET_PALETTE,
    48,
    0,
    0,              // request: 1st palette index  response: 0 if palette is valid, 1 if palette is invalid
    10,             // request: number of palette entries
    0xff000000,     // request: black
    0xff808080,     // request: grey
    0xffffffff,     // request: white
    0xff0000ff,     // request: red
    0xff0080ff,     // request: orange
    0xff00ffff,     // request: yellow
    0xff00ff00,     // request: green
    0xffff0000,     // request: blue
    0xffffff00,     // request: indigo
    0xffff00ff,     // request: violet
    RPI_FIRMWARE_FRAMEBUFFER_ALLOCATE,
    8,
    0,
    4096,          // request: alignment in bytes  response: frame buffer base address or 0 if alignment is unsupported
    0,             // request: padding             response: frame buffer size in bytes or 0 if alignment is unsupported
    RPI_FIRMWARE_PROPERTY_END,
};



// Return pointer to frame buffer or 0 if mailbox request failed to parse
u8* get_fb() {
    mailbox_send(mailbox_init_fb, MAILBOX_CHANNEL_TAGS);
    mailbox_read(MAILBOX_CHANNEL_TAGS);

    if (mailbox_init_fb[1] == RPI_FIRMWARE_STATUS_ERROR) {
        return 0;
    }
    return (u8 *)(mailbox_init_fb[39] & 0x3fffffff);
}



void draw_rect(u8 *fb_base, int x, int y, int width, int height, u8 color) {
    if (x < 0 || 
        y < 0 || 
        width <= 0 || 
        height <= 0 || 
        x + width > SCREEN_WIDTH || 
        y + height > SCREEN_HEIGHT) {
        return;
    }
    u8 *fb = fb_base + (SCREEN_WIDTH * y) + x;

    if (width % 8 == 0) {
        // fast path: draw 8 pixels at a time
        u64 *fb64 = (u64*) fb;

        u64 colors = (u64) color;
        colors |= colors << 8;
        colors |= colors << 16;
        colors |= colors << 32;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width / 8; j++) {
                fb64[j] = colors;
            }
            fb64 += SCREEN_WIDTH / 8;
        }
    } else {
        // slow path: draw one pixel at a time
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                fb[j] = color;
            }
            fb += SCREEN_WIDTH;
        }
    }
}



// Draw scaled character
void draw_char(u8 *fb_base, char c, int x, int y) {
    if (c >= CHAR_COUNT || 
        x < 0 || 
        y < 0 || 
        x + FONT_WIDTH * FONT_SCALE > SCREEN_WIDTH || 
        y + FONT_HEIGHT * FONT_SCALE > SCREEN_HEIGHT) {
        return;
    }
    u8 *fb = fb_base + (SCREEN_WIDTH * y) + x;

    for (int row = 0; row < FONT_HEIGHT * FONT_SCALE; row++) {
        u8 bitmap_row = font_get_char_row(c, row / FONT_SCALE);

        for (int col = 0; col < FONT_WIDTH * FONT_SCALE; col++) {
            fb[col] = bitmap_row & (1 << (col / FONT_SCALE)) ? WHITE : BLACK;
        }
        fb += SCREEN_WIDTH;
    }
}



// Draw scaled string
void draw_string(u8 *fb_base, const char *s, int x, int y) {
    for (int i = 0; s[i] != '\0'; i++) {
        char c = s[i];
        if (c == '\n') {
            x = 0;
            y += FONT_HEIGHT * FONT_SCALE;
        } else {
            draw_char(fb_base, c, x, y);
            x += FONT_WIDTH * FONT_SCALE;
        }
    }
}
