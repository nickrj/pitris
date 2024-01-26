// Minimal Broadcom iProc RNG200 Driver - not for production!
// Reference: https://github.com/raspberrypi/linux/tree/rpi-6.1.y/drivers/char/hw_random/iproc-rng200.c

#include "types.h"
#include "utils.h"
#include "base.h"

#define RNG_CTRL_REG                      (PBASE+0x00104000)
#define RNG_TOTAL_BIT_COUNT_REG           (PBASE+0x0010400C)
#define RNG_TOTAL_BIT_COUNT_THRESHOLD_REG (PBASE+0x00104010)
#define RNG_FIFO_DATA_REG                 (PBASE+0x00104020)
#define RNG_FIFO_COUNT_REG                (PBASE+0x00104024)

#define RNG_CTRL_RNG_DIV_CTRL_SHIFT        13
#define RNG_CTRL_RNG_RBGEN_MASK            0x00001FFF
#define RNG_FIFO_COUNT_RNG_FIFO_COUNT_MASK 0x000000FF


void rand_init() {
    // check if RNG is already enabled
    if (get32(RNG_CTRL_REG) & RNG_CTRL_RNG_RBGEN_MASK) {
        return;
    }

    // generate 0x40000 bits of random data before initialization
    put32(RNG_TOTAL_BIT_COUNT_THRESHOLD_REG, 0x40000);

    // enable RNG and set sample rate to 1Mhz
    put32(RNG_CTRL_REG, (0x3 << RNG_CTRL_RNG_DIV_CTRL_SHIFT) | RNG_CTRL_RNG_RBGEN_MASK);
}


// Return a random number in the range of 0 to (2**31)-1
int rand() {
    // warm up period
    while (get32(RNG_TOTAL_BIT_COUNT_REG) <= 16);

    // wait for random numbers to become available
    while ((get32(RNG_FIFO_COUNT_REG) & RNG_FIFO_COUNT_RNG_FIFO_COUNT_MASK) == 0);

    return get32(RNG_FIFO_DATA_REG) & 0x7fffffff;
}


void shuffle(int *arr, int n) {
    for (int i = n-1; i > 0; i--) {
        int j = rand() % (i+1);

        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}
