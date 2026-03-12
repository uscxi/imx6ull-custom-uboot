#include <asm-offsets.h>
#include <config.h>
#include <linux/linkage.h>
#include <system-constants.h>

.pushsection .text.s_init, "ax"
WEAK(s_init)
	bx	lr
ENDPROC(s_init)
.popsection

.pushsection .text.lowlevel_init, "ax"
WEAK(lowlevel_init)
    ldr	sp, =SYS_INIT_SP_ADDR
    bic	sp, sp, #7 /* 8-byte alignment for ABI compliance */
    sub	sp, sp, #GD_SIZE
	bic	sp, sp, #7
	mov	r9, sp
    /*
	 * Save the old lr(passed in ip) and the current lr to stack
	 */
	push	{ip, lr}
    bl	s_init
	pop	{ip, pc}
ENDPROC(lowlevel_init)
.popsection
