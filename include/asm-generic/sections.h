#ifndef _ASM_GENERIC_SECTIONS_H_
#define _ASM_GENERIC_SECTIONS_H_

#include <linux/types.h>

extern char __bss_start[];
extern char __bss_end[];
extern char _end[];

/*
 * This is the U-Boot entry point - prior to relocation it should be same
 * as __text_start
 */
extern void _start(void);

#ifndef USE_HOSTCC
#if CONFIG_IS_ENABLED(RELOC_LOADER)
#define __rcode __section(".text.rcode")
#define __rdata __section(".text.rdata")
#else
#define __rcode
#define __rdata
#endif
#else
#define __rcode
#define __rdata
#endif

#endif /* _ASM_GENERIC_SECTIONS_H_ */
