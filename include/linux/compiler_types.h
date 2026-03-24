/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_COMPILER_TYPES_H
#define __LINUX_COMPILER_TYPES_H

#ifndef __ASSEMBLY__

#ifdef __KERNEL__

/* Attributes */
#include <linux/compiler_attributes.h>

/* Compiler specific macros. */
#ifdef __GNUC__
/* The above compilers also define __GNUC__, so order is important here. */
#include <linux/compiler-gcc.h>
#else
#error "Unknown compiler"
#endif

#ifdef CONFIG_HAVE_ARCH_COMPILER_H
#include <asm/compiler.h>
#endif

#endif /* __KERNEL__ */

#endif /* __ASSEMBLY__ */

#endif /* __LINUX_COMPILER_TYPES_H */
