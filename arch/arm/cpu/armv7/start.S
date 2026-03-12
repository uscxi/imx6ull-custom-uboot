/*
 * armboot - Startup Code for ARM Cortex CPU-core
 *
 */
#include <config.h>
#include <asm/system.h>
#include <linux/linkage.h>

/*************************************************************************
 *
 * Startup Code (reset vector)
 *
 */
    .global reset
    .global save_boot_params_ret
    .type   save_boot_params_ret,%function

reset:
    /* Allow the board to save important registers */
    b save_boot_params
save_boot_params_ret:
    /*
     * disable interrupts (FIQ and IRQ), also set the cpu to SVC32 mode.
     * SVC模式是CPU上电reset后默认的模式，该模式下主要用来做系统的初始化。
     */
    mrs r0, cpsr @把cpsr寄存器的值传送到r0
    bic r0, r0, #0x1f @将r0寄存器的低5位清零，即cpsr的mode位cpsr[4:0]
    orr r0, r0, #0x13 @r0逻辑或，设置mode位为10011，即SVC模式
    orr r0, r0, #0xc0 @disable FIQ and IRQ. cpsr[7:6]
    msr cpsr, r0 @将r0设置的值回写入cpsr

    /*
     * Setup vector:
     */
    /*
     * Set V=0 in CP15 SCTLR register - for VBAR to point to vector 
     * see Cortex™-A7 MPCore Technical Reference Manual
     * detail: 4.3.27 System Control Register and 4.2.13 c12 register
     */
    mrc p15, 0, r0, c1, c0, 0 @ Read CP15 SCTLR Register
    bic r0, #CR_V @bit[13] v = 0
    mcr	p15, 0, r0, c1, c0, 0	@ Write CP15 SCTLR Register

    /* Set vector address in CP15 VBAR register */
	ldr	r0, =_start
	mcr	p15, 0, r0, c12, c0, 0	@Set VBAR. See c12 register

    /* the mask ROM code should have PLL and others stable */
    bl cpu_init_cp15
    bl cpu_init_crit

    bl _main

/*************************************************************************
 *
 * void save_boot_params(u32 r0, u32 r1, u32 r2, u32 r3)
 *	__attribute__((weak));
 *
 * Stack pointer is not yet initialized at this moment
 * Don't save anything to stack even if compiled with -O0
 *
 *************************************************************************/
WEAK(save_boot_params)
	b	save_boot_params_ret		@ back to my caller
ENDPROC(save_boot_params)

/*************************************************************************
 *
 * cpu_init_cp15
 *
 * Setup CP15 registers (cache, MMU, TLBs). The I-cache is turned on unless
 * CONFIG_SYS_ICACHE_OFF is defined.
 *
 *************************************************************************/
ENTRY(cpu_init_cp15)

    /*
	 * Invalidate L1 I/D
     * TLB（转换查找缓冲区） I-Cache（指令缓存）和BP（分支预测）
	 */
	mov	r0, #0			@ set up for MCR
	mcr	p15, 0, r0, c8, c7, 0	@ invalidate TLBs
	mcr	p15, 0, r0, c7, c5, 0	@ invalidate icache
	mcr	p15, 0, r0, c7, c5, 6	@ invalidate BP array
	dsb @数据同步屏障，等待所有访问完成
	isb @指令同步屏障，清空流水线重新取指

    /*
	 * disable MMU stuff and caches
     * 禁用MMU和数据缓存：简化内存访问，避免缓存一致性问题
     * 启用指令缓存：加速启动代码执行
     * 启用分支预测：提高循环和控制流性能
     * 启用对齐检查：帮助发现代码中的潜在问题
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002000	@ clear bits 13 (--V-) 位13清零
    /* 
     * bit[2:0] CAM
     * bit[2](C):数据缓存使能 (0=禁用, 1=启用),
     * bit[1](A):对齐检查使能 (0=禁用, 1=启用),
     * bit[0](M):MMU使能 (0=禁用, 1=启用)
     */
	bic	r0, r0, #0x00000007	@ clear bits 2:0 (-CAM)

	orr	r0, r0, #0x00000002	@ set bit 1 (--A-) Align 启用对齐检查
	orr	r0, r0, #0x00000800	@ set bit 11 (Z---) BTB 启用分支预测
    orr	r0, r0, #0x00001000	@ set bit 12 (I) I-cache
    mcr	p15, 0, r0, c1, c0, 0
ENDPROC(cpu_init_cp15)

/*************************************************************************
 *
 * CPU_init_critical registers
 *
 * setup important registers
 * setup memory timing
 *
 *************************************************************************/
ENTRY(cpu_init_crit)
	/*
	 * Jump to board specific initialization...
	 * The Mask ROM will have already initialized
	 * basic memory. Go here to bump up clock rate and handle
	 * wake up conditions.
	 */
	b	lowlevel_init		@ go setup pll,mux,memory
ENDPROC(cpu_init_crit)
