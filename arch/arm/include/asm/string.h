#ifndef __ASM_ARM_STRING_H
#define __ASM_ARM_STRING_H

#if CONFIG_IS_ENABLED(USE_ARCH_MEMCPY)
#define __HAVE_ARCH_MEMCPY
#endif
extern void * memcpy(void *, const void *, __kernel_size_t);

#if CONFIG_IS_ENABLED(USE_ARCH_MEMMOVE)
#define __HAVE_ARCH_MEMMOVE
#else
#undef __HAVE_ARCH_MEMMOVE
#endif
extern void * memmove(void *, const void *, __kernel_size_t);

#undef __HAVE_ARCH_MEMCHR
extern void * memchr(const void *, int, __kernel_size_t);

#undef __HAVE_ARCH_MEMZERO
#if CONFIG_IS_ENABLED(USE_ARCH_MEMSET)
#define __HAVE_ARCH_MEMSET
#endif
extern void * memset(void *, int, __kernel_size_t);

#endif
