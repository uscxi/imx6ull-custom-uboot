#ifndef __ARCH_ARM_POSIX_TYPES_H
#define __ARCH_ARM_POSIX_TYPES_H

#ifdef	__aarch64__
typedef unsigned long		__kernel_size_t;
typedef long			__kernel_ssize_t;
typedef long			__kernel_ptrdiff_t;
#else
typedef unsigned int		__kernel_size_t;
typedef int			__kernel_ssize_t;
typedef int			__kernel_ptrdiff_t;
#endif

#endif
