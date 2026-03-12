
    .global _start

_start:
    b   reset                  @ 直接跳转，无需地址表
    b   undefined_instruction
    b   software_interrupt
    b   prefetch_abort
    b   data_abort
    b   not_used
    b   irq
    b   fiq
    
/*
 * 异常处理桩函数
 * 在U-Boot启动阶段，这些异常不应该发生
 * 如果发生，说明代码有严重错误，进入死循环便于调试
 */
reset:
undefined_instruction:
software_interrupt:
prefetch_abort:
data_abort:
not_used:
irq:
fiq:
    /* 
     * 进入无限循环
     * 在调试时可以通过JTAG查看PC值，判断触发了哪个异常
     * QEMU调试时可以直接看PC值
     */
1:  b       1b                      /* 跳转到自身，形成死循环 */
