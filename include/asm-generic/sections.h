#ifndef _ASM_GENERIC_SECTIONS_H_
#define _ASM_GENERIC_SECTIONS_H_

#include <linux/types.h>

extern char __bss_start[];
extern char __bss_end[];

/*
 * This is the U-Boot entry point - prior to relocation it should be same
 * as __text_start
 */
extern void _start(void);

#endif /* _ASM_GENERIC_SECTIONS_H_ */
