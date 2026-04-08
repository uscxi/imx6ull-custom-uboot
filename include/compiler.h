#ifndef __COMPILER_H__
#define __COMPILER_H__

#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

#ifdef __LP64__
#define MEM_SUPPORT_64BIT_DATA	1
#else
#define MEM_SUPPORT_64BIT_DATA	0
#endif

#endif
