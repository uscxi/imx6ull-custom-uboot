#include <config.h>
#include <asm-offsets.h>
#include <linux/linkage.h>
#include <system-constants.h>

/*
 * 此文件处理U-Boot启动中与目标平台无关的阶段，这些阶段需要C运行时环境。
 * 其入口点为_main，由目标平台的start.S文件跳转进入。
 */

/*
 * entry point of crt0 sequence
 */
/*定义函数入口，从start.S跳转过来*/
ENTRY(_main)

/*
 * Set up initial C runtime environment and call board_init_f(0).
 */
    ldr r0, =(SYS_INIT_SP_ADDR)  /* 正常U-Boot使用的栈地址 */
    /*清除低 3 位，ARM EABI 要求栈 8 字节对齐。*/
    bic	r0, r0, #7	/* 8-byte alignment for ABI compliance */
    /*设置栈指针，有了基本栈，可以调用 C 函数了！*/
    mov	sp, r0

/*早期串口初始化，便于调试输出*/
#if defined(CONFIG_DEBUG_UART) && CONFIG_IS_ENABLED(SERIAL)
    bl	debug_uart_init
#endif

    bl	main_loop

ENDPROC(_main)
