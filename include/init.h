#ifndef __INIT_H_
#define __INIT_H_	1

#ifndef __ASSEMBLY__		/* put C only stuff in this section */

#include <linux/types.h>
#include <asm/io.h>
#include <debug_uart.h>
#include <linux/string.h>

/* common/board_f.c */
void board_init_f(ulong dummy);

/**
 * ulong board_init_f_alloc_reserve - allocate reserved area
 * @top: top of the reserve area, growing down.
 *
 * This function is called by each architecture very early in the start-up
 * code to allow the C runtime to reserve space on the stack for writable
 * 'globals' such as GD and the malloc arena.
 *
 * Return: bottom of reserved area
 */
ulong board_init_f_alloc_reserve(ulong top);

/**
 * board_init_f_init_reserve - initialize the reserved area(s)
 * @base:	top from which reservation was done
 *
 * This function is called once the C runtime has allocated the reserved
 * area on the stack. It must initialize the GD at the base of that area.
 */
void board_init_f_init_reserve(ulong base);

struct global_data;

/**
 * arch_setup_gd() - Set up the global_data pointer
 * @gd_ptr: Pointer to global data
 *
 * This pointer is special in some architectures and cannot easily be assigned
 * to. For example on x86 it is implemented by adding a specific record to its
 * Global Descriptor Table! So we we provide a function to carry out this task.
 * For most architectures this can simply be:
 *
 *    gd = gd_ptr;
 */
void arch_setup_gd(struct global_data *gd_ptr);

/**
 * main_loop() - Enter the main loop of U-Boot
 *
 * This normally runs the command line.
 */
void main_loop(void);

#endif	/* __ASSEMBLY__ */
/* Put only stuff here that the assembler can digest */

#endif	/* __INIT_H_ */
