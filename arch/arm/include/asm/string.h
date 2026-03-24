#ifndef __ASM_ARM_STRING_H
#define __ASM_ARM_STRING_H

#undef __HAVE_ARCH_MEMZERO
#if CONFIG_IS_ENABLED(USE_ARCH_MEMSET)
#define __HAVE_ARCH_MEMSET
#endif
extern void * memset(void *, int, __kernel_size_t);

#endif
