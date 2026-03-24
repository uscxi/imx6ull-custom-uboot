#ifndef __ASM_GENERIC_U_BOOT_H__
#define __ASM_GENERIC_U_BOOT_H__

#ifndef __ASSEMBLY__

#include <asm/types.h>
#include <linux/types.h>

struct bd_info {
	unsigned long	bi_flashstart;	/* start of FLASH memory */
	unsigned long	bi_flashsize;	/* size	 of FLASH memory */
	unsigned long	bi_flashoffset; /* reserved area for startup monitor */
};

#endif /* __ASSEMBLY__ */

#endif	/* __ASM_GENERIC_U_BOOT_H__ */
