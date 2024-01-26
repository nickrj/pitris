#include "utils.h"
#include "types.h"
#include "base.h"

#define GPFSEL1         (PBASE+0x00200004)
#define GPSET0          (PBASE+0x0020001C)
#define GPCLR0          (PBASE+0x00200028)
#define GPPUD           (PBASE+0x00200094)
#define GPPUDCLK0       (PBASE+0x00200098)

#define AUX_ENABLES     (PBASE+0x00215004)
#define AUX_MU_IO_REG   (PBASE+0x00215040)
#define AUX_MU_IER_REG  (PBASE+0x00215044)
#define AUX_MU_IIR_REG  (PBASE+0x00215048)
#define AUX_MU_LCR_REG  (PBASE+0x0021504C)
#define AUX_MU_MCR_REG  (PBASE+0x00215050)
#define AUX_MU_LSR_REG  (PBASE+0x00215054)
#define AUX_MU_MSR_REG  (PBASE+0x00215058)
#define AUX_MU_SCRATCH  (PBASE+0x0021505C)
#define AUX_MU_CNTL_REG (PBASE+0x00215060)
#define AUX_MU_STAT_REG (PBASE+0x00215064)
#define AUX_MU_BAUD_REG (PBASE+0x00215068)


void uart_send(char c) {
    while (true) {
        if (get32(AUX_MU_LSR_REG) & 0x20) {
            break;
        }
    }
    put32(AUX_MU_IO_REG, c);
}


char uart_read() {
    if (get32(AUX_MU_LSR_REG) & 0x01) {
        return get32(AUX_MU_IO_REG) & 0xff;
    }
    return 0;
}


void uart_init() {
    unsigned int selector;

    selector = get32(GPFSEL1);
    selector &= ~(7<<12);                   // clean gpio14
    selector |= 2<<12;                      // set alt5 for gpio14
    selector &= ~(7<<15);                   // clean gpio15
    selector |= 2<<15;                      // set alt5 for gpio15
    put32(GPFSEL1, selector);

    put32(GPPUD, 0);
    delay(150);
    put32(GPPUDCLK0, (1<<14) | (1<<15));
    delay(150);
    put32(GPPUDCLK0, 0);

    put32(AUX_ENABLES, 1);                  // enable mini uart (this also enables access to its registers)
    put32(AUX_MU_CNTL_REG, 0);              // disable auto flow control and disable receiver and transmitter
    put32(AUX_MU_IER_REG, 0);               // disable receive and transmit interrupts
    put32(AUX_MU_LCR_REG, 3);               // enable 8 bit mode
    put32(AUX_MU_MCR_REG, 0);               // set RTS line to be always high
    put32(AUX_MU_BAUD_REG, 541);            // set baud rate to 115200 (GPU is now 500Mhz for RPi 4)

    put32(AUX_MU_CNTL_REG, 3);              // finally, enable transmitter and receiver
}
