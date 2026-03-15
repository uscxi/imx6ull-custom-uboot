#ifndef __ASM_ARCH_MX6_IMX_REGS_H__
#define __ASM_ARCH_MX6_IMX_REGS_H__

#define IRAM_BASE_ADDR			0x00900000 // OCRAM基地址
#define MMDC0_ARB_BASE_ADDR     0x80000000 // MMDC—x16 DDR Controller.
#define MMDC0_ARB_END_ADDR              0xFFFFFFFF

#define AIPS1_ARB_BASE_ADDR             0x02000000
#define AIPS1_ARB_END_ADDR              0x020FFFFF

/* Defines for Blocks connected via AIPS (SkyBlue) */
#define ATZ1_BASE_ADDR              AIPS1_ARB_BASE_ADDR

#define IRAM_SIZE               0x00020000 // 对应手册上的OCRAM 128 KB

#define AIPS1_OFF_BASE_ADDR         (ATZ1_BASE_ADDR + 0x80000)
#define CCM_BASE_ADDR               (AIPS1_OFF_BASE_ADDR + 0x44000)

#define UART1_BASE                  (ATZ1_BASE_ADDR + 0x20000)

/* 
 * CCM相关寄存器地址 
 */
#define CCM_CCGR0 			*((volatile unsigned int *)0X020C4068)
#define CCM_CCGR1 			*((volatile unsigned int *)0X020C406C)

#define CCM_CCGR2 			*((volatile unsigned int *)0X020C4070)
#define CCM_CCGR3 			*((volatile unsigned int *)0X020C4074)
#define CCM_CCGR4 			*((volatile unsigned int *)0X020C4078)
#define CCM_CCGR5 			*((volatile unsigned int *)0X020C407C)
#define CCM_CCGR6 			*((volatile unsigned int *)0X020C4080)

/* 
 * IOMUX相关寄存器地址 
 */
#define SW_MUX_GPIO1_IO03 	*((volatile unsigned int *)0X020E0068)
#define SW_PAD_GPIO1_IO03 	*((volatile unsigned int *)0X020E02F4)

/* 
 * GPIO1相关寄存器地址 
 */
#define GPIO1_DR 			*((volatile unsigned int *)0X0209C000)
#define GPIO1_GDIR 			*((volatile unsigned int *)0X0209C004)
#define GPIO1_PSR 			*((volatile unsigned int *)0X0209C008)
#define GPIO1_ICR1 			*((volatile unsigned int *)0X0209C00C)
#define GPIO1_ICR2 			*((volatile unsigned int *)0X0209C010)
#define GPIO1_IMR 			*((volatile unsigned int *)0X0209C014)
#define GPIO1_ISR 			*((volatile unsigned int *)0X0209C018)
#define GPIO1_EDGE_SEL 		*((volatile unsigned int *)0X0209C01C)

#endif /* __ASM_ARCH_MX6_IMX_REGS_H__ */
