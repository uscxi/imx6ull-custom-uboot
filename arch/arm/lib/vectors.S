/*
 *  vectors - Generic ARM exception table code
 */

    /* 
     * 宏定义
     * 0x00: 复位异常
     * 0x04: 未定义指令异常
     * 0x08: 软中断（SWI/SVC）
     * 0x0C: 预取指中止
     * 0x10: 数据访问中止
     * 0x14: 保留（未使用）
     * 0x18: IRQ 中断
     * 0x1C: FIQ 快速中断
     */
    .macro ARM_VECTORS
	ldr pc, _reset
	ldr pc, _undefined_instruction
	ldr pc, _software_interrupt
	ldr pc, _prefetch_abort
	ldr pc, _data_abort
	ldr pc, _not_used
	ldr pc, _irq
	ldr pc, _fiq
    .endm

.globl _start         /* 将 _start 声明为全局符号，链接器可以找到它 */

.section ".vectors", "ax"   @ 放入名为 ".vectors" 的段
                            @ "a" = allocatable（可分配）
                            @ "x" = executable（可执行）
/* 程序向量表入口 */
_start:
    ARM_VECTORS        @ 展开宏，生成8条 ldr 指令

	@ .globl 将符号声明为全局可见，使其能被其他源文件访问。
	.globl  _reset
	.globl	_undefined_instruction
	.globl	_software_interrupt
	.globl	_prefetch_abort
	.globl	_data_abort
	.globl	_not_used
	.globl	_irq
	.globl	_fiq

_reset:			.word reset
_undefined_instruction:	.word undefined_instruction
_software_interrupt:	.word software_interrupt
_prefetch_abort:	.word prefetch_abort
_data_abort:		.word data_abort
_not_used:		.word not_used
_irq:			.word irq
_fiq:			.word fiq

	.balignl 16, 0xdeadbeef @ 按16字节对齐，并用0xdeadbeef填充空白。

	.align	5                    @ 对齐到 2^5 = 32 字节边界
undefined_instruction:
software_interrupt:
prefetch_abort:
data_abort:
not_used:
irq:
fiq:
1:
	bl	1b                   @ 跳转到标签 "1"（即自身），无限循环
