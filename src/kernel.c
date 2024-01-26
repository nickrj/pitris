#include "mini_uart.h"
#include "framebuf.h"
#include "types.h"
#include "printf.h"
#include "rand.h"
#include "game.h"


void putc(void *p, char c) {
    if (c == '\n') {
        uart_send('\r');
    }
    uart_send(c);
}


void kernel_main() {
    uart_init();
    init_printf(0, putc);
    printf("Initializing TETRIS OS...\n");

    printf("Initializing hardware RNG...\n");
    rand_init();

    printf("Sending mailbox request...\n");
    u8 *fb_base = get_fb();

    if (fb_base == 0) {
        printf("Error parsing mailbox request\n");
        return;
    }
    printf("Loading game...\n");
    start_game(fb_base);
}
