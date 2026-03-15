#ifndef __INIT_H_
#define __INIT_H_	1

#ifndef __ASSEMBLY__		/* put C only stuff in this section */

#include <linux/types.h>
#include <debug_uart.h>

/**
 * main_loop() - Enter the main loop of U-Boot
 *
 * This normally runs the command line.
 */
void main_loop(void);

#endif	/* __ASSEMBLY__ */
/* Put only stuff here that the assembler can digest */

#endif	/* __INIT_H_ */
