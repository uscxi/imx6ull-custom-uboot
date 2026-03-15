#ifndef _LINUX_BITOPS_H
#define _LINUX_BITOPS_H

#if !defined(USE_HOSTCC) && !defined(__ASSEMBLY__)

#ifdef	__KERNEL__
#define BIT(nr)			(1UL << (nr))
#define BIT_ULL(nr)		(1ULL << (nr))
#endif

#endif /* !USE_HOSTCC && !__ASSEMBLY__ */

#endif
