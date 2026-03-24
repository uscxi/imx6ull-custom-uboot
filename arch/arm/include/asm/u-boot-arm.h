#ifndef _U_BOOT_ARM_H_
#define _U_BOOT_ARM_H_	1

#ifndef __ASSEMBLY__

/* for the following variables, see start.S */
extern ulong IRQ_STACK_START;	/* top of IRQ stack */
extern ulong FIQ_STACK_START;	/* top of FIQ stack */
extern ulong _datarel_start_ofs;
extern ulong _datarelrolocal_start_ofs;
extern ulong _datarellocal_start_ofs;
extern ulong _datarelro_start_ofs;
extern ulong IRQ_STACK_START_IN;	/* 8 bytes in IRQ stack */

void s_init(void);

/* Set up ARMv7 MMU, caches and TLBs */
void	cpu_init_cp15(void);

#endif /* __ASSEMBLY__ */

#endif	/* _U_BOOT_ARM_H_ */
