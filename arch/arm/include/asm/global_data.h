/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2002-2010
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

#ifndef	__ASM_GBL_DATA_H
#define __ASM_GBL_DATA_H

#ifndef __ASSEMBLY__

#include <config.h>

/* Architecture-specific global data */
struct arch_global_data {
};

#include <asm-generic/global_data.h>

#if defined(__clang__) || defined(LTO_ENABLE)

#define DECLARE_GLOBAL_DATA_PTR
#define gd	get_gd()

static inline gd_t *get_gd(void)
{
gd_t *gd_ptr;

#ifdef CONFIG_ARM64
    _asm__ volatile("mov %0, x18\n" : "=r" (gd_ptr));
#else
    __asm__ volatile("mov %0, r9\n" : "=r" (gd_ptr));
 #endif

    return gd_ptr;
}

#else

#ifdef CONFIG_ARM64
#define DECLARE_GLOBAL_DATA_PTR		register volatile gd_t *gd asm ("x18")
#else
#define DECLARE_GLOBAL_DATA_PTR		register volatile gd_t *gd asm ("r9")
#endif
#endif

static inline void set_gd(volatile gd_t *gd_ptr)
{
#ifdef CONFIG_ARM64
	__asm__ volatile("ldr x18, %0\n" : : "m"(gd_ptr));
#elif __ARM_ARCH >= 7
	__asm__ volatile("ldr r9, %0\n" : : "m"(gd_ptr));
#else
	__asm__ volatile("mov r9, %0\n" : : "r"(gd_ptr));
#endif
}

#endif /* __ASSEMBLY__ */

#endif /* __ASM_GBL_DATA_H */
