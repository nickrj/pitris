#ifndef	_BOOT_H
#define	_BOOT_H

#include "types.h"

extern void delay(u64);
extern void put32(u64, u32);
extern u32 get32(u64);

#endif  /*_BOOT_H */
