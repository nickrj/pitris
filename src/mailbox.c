#include "types.h"
#include "mailbox.h"
#include "utils.h"
#include "base.h"

#define MAILBOX_READ   (PBASE+0x0000B880)
#define MAILBOX_POLL   (PBASE+0x0000B890)
#define MAILBOX_SENDER (PBASE+0x0000B894)
#define MAILBOX_STATUS (PBASE+0x0000B898)
#define MAILBOX_CONFIG (PBASE+0x0000B89C)
#define MAILBOX_WRITE  (PBASE+0x0000B8A0)

#define MAILBOX_EMPTY  0x40000000
#define MAILBOX_FULL   0x80000000


void mailbox_send(u32 *buf, int ch) {
    while (get32(MAILBOX_STATUS) & MAILBOX_FULL);

    put32(MAILBOX_WRITE, ((u32) buf & 0xFFFFFFF0) | (ch & 0xF));
}


u32* mailbox_read(int ch) {
    while (true) {
        while (get32(MAILBOX_STATUS) & MAILBOX_EMPTY);

        u32 msg = get32(MAILBOX_READ);

        if ((msg & 0xF) == ch) {
            return (u32 *)(msg & 0xFFFFFFF0);
        }
    }
}
