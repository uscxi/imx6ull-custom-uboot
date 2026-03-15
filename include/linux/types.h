#ifndef _LINUX_TYPES_H
#define _LINUX_TYPES_H

#include <linux/posix_types.h>
#include <asm/types.h>
#include <stdbool.h>

/* sysv */
typedef unsigned char		uchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;

/*
 * The following typedefs are also protected by individual ifdefs for
 * historical reasons:
 */
#ifndef _SIZE_T
#define _SIZE_T
typedef __kernel_size_t		size_t;
#endif

#endif /* _LINUX_TYPES_H */
