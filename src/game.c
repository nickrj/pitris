#include "mini_uart.h"
#include "framebuf.h"
#include "timer.h"
#include "types.h"
#include "rand.h"
#include "printf.h"

// NOTE: cell size should be a multiple of 8 in order to draw quickly
#define CELL_SIZE        24     

#define GRID_CELL_WIDTH  10
#define GRID_CELL_HEIGHT 20

#define GRID_WIDTH       (GRID_CELL_WIDTH * CELL_SIZE)
#define GRID_HEIGHT      (GRID_CELL_HEIGHT * CELL_SIZE)

#define GRID_X           ((SCREEN_WIDTH - GRID_WIDTH) / 2)
#define GRID_Y           ((SCREEN_HEIGHT - GRID_HEIGHT) / 2)


static u8 grid[GRID_CELL_HEIGHT][GRID_CELL_WIDTH] = {{BLACK}};
 
static u8 block_colors[7] = {RED, ORANGE, YELLOW, GREEN, BLUE, INDIGO, VIOLET};

static u8 blocks[7][4][4][4] = {
    {
        // T block
        {
            {0, 1, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
        {
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0},
        },
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0},
        },
        {
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0},
        },
    },
    {
        // Z block
        {
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
        {
            {0, 0, 1, 0},
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0},
        },
        {
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
        {
            {0, 0, 1, 0},
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0},
        },
    },
    {
        // reverse Z block
        {
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
        {
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 0},
        },
        {
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
        {
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 0},
        },
    },
    {
        // L block
        {
            {0, 0, 1, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
        {
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
        },
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {1, 0, 0, 0},
            {0, 0, 0, 0},
        },
        {
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0},
        },
    },
    {
        // reverse L block
        {
            {1, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
        {
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0},
        },
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 0},
        },
        {
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
        },
    },
    {
        // square block
        {
            {1, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
        {
            {1, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
        {
            {1, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
        {
            {1, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
    },
    {
        // flat block
        {
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
        {
            {0, 0, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 1, 0},
        },
        {
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
        {
            {0, 0, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 1, 0},
        },
    },
};



typedef struct {
    int grid_x;
    int grid_y;
    int rot_index;
    int block_index;
    u8 (*data)[4];
    u8 color;
} Block;


// Game state
typedef struct {
    int score;
    int level;
    int lines_cleared;
    int ms;
    int queue_index;
    int block_queue[14];
    Block b;
} Game;



// Return true if block is outside the grid, or overlapping with another grid cell
// It is valid for a block to be directly above the grid
static bool is_colliding(Block *b) {
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            int grid_x = b->grid_x + col;
            int grid_y = b->grid_y + row;

            if (b->data[row][col] == 1 && 
                (grid_x < 0 ||
                 grid_x >= GRID_CELL_WIDTH || 
                 grid_y >= GRID_CELL_HEIGHT || 
                 (grid_y >= 0 && grid[grid_y][grid_x] != BLACK))) {
                return true;
            }

        }
    }
    return false;
}



static bool move_block(Block *b, int dx, int dy) {
    b->grid_x += dx;
    b->grid_y += dy;

    if (is_colliding(b)) {
        b->grid_x -= dx;
        b->grid_y -= dy;
        return false;
    }
    return true;
}



// Rotate block left/right
static bool rotate_block(Block *b, bool rotate_right) {
    int r = rotate_right ? 1 : -1;

    b->rot_index = (b->rot_index + r) & 3;
    b->data = blocks[b->block_index][b->rot_index];

    if (is_colliding(b)) {
        b->rot_index = (b->rot_index - r) & 3;
        b->data = blocks[b->block_index][b->rot_index];
        return false;
    }
    return true;
}



// Return true if block has landed
// Block is assumed to be in a valid grid position
static bool block_landed(Block b) {
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            int grid_x = b.grid_x + col;
            int grid_y = b.grid_y + row;

            if (b.data[row][col] == 1 && (grid_y == GRID_CELL_HEIGHT - 1 || grid[grid_y+1][grid_x] != BLACK)) {
                return true;
            }
        }
    }
    return false;
}



// Mask block within grid
static void mask_block_to_grid(Block b) {
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            int grid_x = b.grid_x + col;
            int grid_y = b.grid_y + row;

            if (b.data[row][col] == 1 && 
                grid_x >= 0 && 
                grid_x < GRID_CELL_WIDTH && 
                grid_y >= 0 && 
                grid_y < GRID_CELL_HEIGHT) {
                grid[grid_y][grid_x] = b.color;
            }
        }
    }
}



// Return the total points for the current grid and level
// 1 line: 100 points
// 2 lines: 300 points
// 3 lines: 500 points
// 4 lines: 800 points
static int get_points(int level) {
    int points = 0;
    int num_full_lines = 0;

    for (int row = 0; row < GRID_CELL_HEIGHT; row++) {
        bool line_full = true;

        for (int col = 0; col < GRID_CELL_WIDTH; col++) {
            if (grid[row][col] == BLACK) {
                line_full = false;
                break;
            }
        }

        if (line_full) {
            num_full_lines += 1;

            if (num_full_lines == 1) {
                points += 100 * level;
            } else if (num_full_lines == 2) {
                points += 200 * level;
            } else if (num_full_lines == 3) {
                points += 200 * level;
            } else if (num_full_lines == 4) {
                points += 300 * level;
            }
        } else {
            num_full_lines = 0;
        }
    }
    return points;
}



// Remove completed lines from the grid and move blocks down
// Returns the number of lines cleared
static int clear_grid_lines() {
    int lines_cleared = 0;
    int row = GRID_CELL_HEIGHT - 1;

    while (row >= 0) {
        bool line_full = true;

        for (int col = 0; col < GRID_CELL_WIDTH; col++) {
            if (grid[row][col] == BLACK) {
                line_full = false;
            }
        }

        if (line_full) {
            if (row == 0) {
                for (int col = 0; col < GRID_CELL_WIDTH; col++) {
                    grid[row][col] = BLACK;
                }
            } else {
                for (int tmp_row = row; tmp_row > 0; tmp_row--) {
                    for (int col = 0; col < GRID_CELL_WIDTH; col++) {
                        grid[tmp_row][col] = grid[tmp_row-1][col];
                    }
                }
            }
            lines_cleared++;
        } else {
            row -= 1;
        }
    }
    return lines_cleared;
}



static Block spawn_block(int block_index, int grid_x, int grid_y) {
    Block b;
    b.grid_x = grid_x;
    b.grid_y = grid_y;
    b.rot_index = 0;
    b.block_index = block_index;
    b.data = blocks[block_index][0];
    b.color = block_colors[block_index];
    return b;
}



#define STROKE_WIDTH 5
#define TITLE_X GRID_X - (CELL_SIZE * 8)
#define SCORE_X GRID_X + GRID_WIDTH + (CELL_SIZE * 2)


// Draw title, controls, initial score, initial level and bounding box
static void draw_background(u8* fb_base) {
    draw_string(fb_base, "TETRIS OS",    TITLE_X, GRID_Y + 10);

    draw_string(fb_base, "CONTROLS",     TITLE_X, GRID_Y + 70);
    draw_string(fb_base, "W ROTATE",     TITLE_X, GRID_Y + 110);
    draw_string(fb_base, "A LEFT",       TITLE_X, GRID_Y + 135);
    draw_string(fb_base, "S DROP",       TITLE_X, GRID_Y + 160);
    draw_string(fb_base, "D RIGHT",      TITLE_X, GRID_Y + 185);
    draw_string(fb_base, "Q ROTATE",     TITLE_X, GRID_Y + 210);
    draw_string(fb_base, "SPACE START",  TITLE_X, GRID_Y + 235);

    draw_string(fb_base, "000000", SCORE_X, GRID_Y + 10);
    draw_string(fb_base, "LEVEL 1", SCORE_X, GRID_Y + GRID_HEIGHT - 25);

    // bounding box
    draw_rect(
        fb_base,
        GRID_X - STROKE_WIDTH,
        GRID_Y - STROKE_WIDTH,
        GRID_WIDTH + (STROKE_WIDTH * 2),
        GRID_HEIGHT + (STROKE_WIDTH * 2),
        GREY
    );
    draw_rect(fb_base, GRID_X, GRID_Y, GRID_WIDTH, GRID_HEIGHT, BLACK);
}



static void draw_block(u8 *fb_base, Block b) {
    int x = GRID_X + b.grid_x * CELL_SIZE;
    int y = GRID_Y + b.grid_y * CELL_SIZE;

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            u8 color = (b.data[row][col] == 0) ? BLACK : b.color;
            draw_rect(fb_base, x + col * CELL_SIZE, y + row * CELL_SIZE, CELL_SIZE, CELL_SIZE, color);
        }
    }
}



static void draw_grid(u8 *fb_base) {
    for (int row = 0; row < GRID_CELL_HEIGHT; row++) {
        for (int col = 0; col < GRID_CELL_WIDTH; col++) {
            int x = GRID_X + col * CELL_SIZE;
            int y = GRID_Y + row * CELL_SIZE;

            draw_rect(fb_base, x, y, CELL_SIZE, CELL_SIZE, grid[row][col]);
        }
    }
}



// Draw grid, queued blocks, current score and level
static void draw_game(u8 *fb_base, Game game) {
    //u64 start = timer_get_counter();

    mask_block_to_grid(game.b);
    draw_grid(fb_base);

    // queued blocks
    draw_block(fb_base, spawn_block(game.block_queue[game.queue_index+1], GRID_CELL_WIDTH + 2, 4));
    draw_block(fb_base, spawn_block(game.block_queue[game.queue_index+2], GRID_CELL_WIDTH + 2, 8));
    draw_block(fb_base, spawn_block(game.block_queue[game.queue_index+3], GRID_CELL_WIDTH + 2, 12));

    char s[11];
    sprintf(s, "%06d", game.score);
    draw_string(fb_base, s, SCORE_X, GRID_Y + 10);

    sprintf(s, "LEVEL %d", game.level);
    draw_string(fb_base, s, SCORE_X, GRID_Y + GRID_HEIGHT - 25);

    // remove block from grid
    game.b.color = BLACK;
    mask_block_to_grid(game.b);

    //printf("total frame time: %d ms\n", (timer_get_counter() - start) / 1000);
}



static bool is_game_over() {
    for (int col = 0; col < GRID_CELL_WIDTH; col++) {
        if (grid[0][col] != BLACK) {
            return true;
        }
    }
    return false;
}



static void reset_grid() {
    for (int row = 0; row < GRID_CELL_HEIGHT; row++) {
        for (int col = 0; col < GRID_CELL_WIDTH; col++) {
            grid[row][col] = BLACK;
        }
    }
}



#define NUM_LEVELS 18

// Cache the time in milliseconds for a piece to fall down by one row for each level
// Formula: ((0.8-((level-1)*0.007))**(level-1)) * 1000
static int ms_level[NUM_LEVELS] = {1000, 793, 617, 472, 355, 262, 189, 134, 93, 64, 42, 28, 18, 11, 7, 4, 2, 1};



static Game init_game() {
    Game game;
    game.score = 0;
    game.level = 1;
    game.lines_cleared = 0;
    game.ms = 1000;
    game.queue_index = 0;

    for (int i = 0; i < 14; i++) {
        game.block_queue[i] = i % 7;
    }

    shuffle(game.block_queue, 7);            // current block queue
    shuffle(&game.block_queue[7], 7);        // next block queue

    game.b = spawn_block(game.block_queue[0], 4, -2);

    return game;
}



void start_game(u8 *fb_base) {
    draw_background(fb_base);

    Game game = init_game();

    printf("Press space to start the game\n");
    while (uart_read() != ' ');
    printf("Starting game!\n");

    while (true) {
        move_block(&game.b, 0, 1);
        draw_game(fb_base, game);

        // move/rotate block within current time interval
        u64 end = timer_get_counter() + (game.ms * 1000);

        while ((s64)(end - timer_get_counter()) > 0) {
            char c = uart_read();
            bool reset = false;

            if (c == 's') {
                reset = move_block(&game.b, 0, 1);
            } else if (c == 'a') {
                reset = move_block(&game.b, -1, 0);
            } else if (c == 'd') {
                reset = move_block(&game.b, 1, 0);
            } else if (c == 'w') {
                reset = rotate_block(&game.b, true);
            } else if (c == 'q') {
                reset = rotate_block(&game.b, false);
            } else {
                continue;
            }

            if (reset) {
                draw_game(fb_base, game);

                if (block_landed(game.b)) {
                    // reset timer
                    end = timer_get_counter() + (game.ms * 1000);
                }
            }
        }

        if (block_landed(game.b)) {
            mask_block_to_grid(game.b);

            game.score += get_points(game.level);
            game.lines_cleared += clear_grid_lines();
            game.level = (game.lines_cleared / 10) + 1;

            if (game.level-1 < NUM_LEVELS) {
                game.ms = ms_level[game.level-1];
            }
            game.queue_index = (game.queue_index + 1) % 7;

            if (game.queue_index == 0) {
                // copy next queue into current queue and shuffle next queue
                for (int i = 0; i < 7; i++) {
                    game.block_queue[i] = game.block_queue[i+7];
                }
                shuffle(&game.block_queue[7], 7);
            }
            game.b = spawn_block(game.block_queue[game.queue_index], 4, -2);

            if (is_game_over()) {
                printf("Game over! High score: %d\nPress space to try again\n", game.score);
                while (uart_read() != ' ');
                game = init_game();
                reset_grid();
            }

            draw_game(fb_base, game);
        }
    }
}
