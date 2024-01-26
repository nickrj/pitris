#include "utils.h"
#include "types.h"
#include "base.h"

#define SYS_TIMER_CS_REG  (PBASE+0x00003000)
#define SYS_TIMER_CLO_REG (PBASE+0x00003004)
#define SYS_TIMER_CHI_REG (PBASE+0x00003008)
#define SYS_TIMER_C0_REG  (PBASE+0x0000300c)
#define SYS_TIMER_C1_REG  (PBASE+0x00003010)
#define SYS_TIMER_C2_REG  (PBASE+0x00003014)
#define SYS_TIMER_C3_REG  (PBASE+0x00003018)

#define CLOCK_HZ 1000000


u64 timer_get_counter() {
    u32 hi = get32(SYS_TIMER_CHI_REG);
    u32 lo = get32(SYS_TIMER_CLO_REG);

    // repeat if hi changed
    if (hi != get32(SYS_TIMER_CHI_REG)) {
        hi = get32(SYS_TIMER_CHI_REG);
        lo = get32(SYS_TIMER_CLO_REG);
    }

    return ((u64) hi << 32) | lo;
}
