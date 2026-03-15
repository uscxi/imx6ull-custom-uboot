// SPDX-License-Identifier: GPL-2.0+

#include <asm/arch/imx-regs.h>
#include <asm/io.h>
#include <linux/bitops.h>

/* UART Control Register Bit Fields.*/
#define URXD_CHARRDY	(1<<15)
#define URXD_ERR	(1<<14)
#define URXD_OVRRUN	(1<<13)
#define URXD_FRMERR	(1<<12)
#define URXD_BRK	(1<<11)
#define URXD_PRERR	(1<<10)
#define URXD_RX_DATA	(0xFF)
#define UCR1_ADEN	(1<<15) /* Auto dectect interrupt */
#define UCR1_ADBR	(1<<14) /* Auto detect baud rate */
#define UCR1_TRDYEN	(1<<13) /* Transmitter ready interrupt enable */
#define UCR1_IDEN	(1<<12) /* Idle condition interrupt */
#define UCR1_RRDYEN	(1<<9)	/* Recv ready interrupt enable */
#define UCR1_RDMAEN	(1<<8)	/* Recv ready DMA enable */
#define UCR1_IREN	(1<<7)	/* Infrared interface enable */
#define UCR1_TXMPTYEN	(1<<6)	/* Transimitter empty interrupt enable */
#define UCR1_RTSDEN	(1<<5)	/* RTS delta interrupt enable */
#define UCR1_SNDBRK	(1<<4)	/* Send break */
#define UCR1_TDMAEN	(1<<3)	/* Transmitter ready DMA enable */
#define UCR1_UARTCLKEN	(1<<2)	/* UART clock enabled */
#define UCR1_DOZE	(1<<1)	/* Doze */
#define UCR1_UARTEN	(1<<0)	/* UART enabled */
#define UCR2_ESCI	(1<<15) /* Escape seq interrupt enable */
#define UCR2_IRTS	(1<<14) /* Ignore RTS pin */
#define UCR2_CTSC	(1<<13) /* CTS pin control */
#define UCR2_CTS	(1<<12) /* Clear to send */
#define UCR2_ESCEN	(1<<11) /* Escape enable */
#define UCR2_PREN	(1<<8)  /* Parity enable */
#define UCR2_PROE	(1<<7)  /* Parity odd/even */
#define UCR2_STPB	(1<<6)	/* Stop */
#define UCR2_WS		(1<<5)	/* Word size */
#define UCR2_RTSEN	(1<<4)	/* Request to send interrupt enable */
#define UCR2_TXEN	(1<<2)	/* Transmitter enabled */
#define UCR2_RXEN	(1<<1)	/* Receiver enabled */
#define UCR2_SRST	(1<<0)	/* SW reset */
#define UCR3_DTREN	(1<<13) /* DTR interrupt enable */
#define UCR3_PARERREN	(1<<12) /* Parity enable */
#define UCR3_FRAERREN	(1<<11) /* Frame error interrupt enable */
#define UCR3_DSR	(1<<10) /* Data set ready */
#define UCR3_DCD	(1<<9)  /* Data carrier detect */
#define UCR3_RI		(1<<8)  /* Ring indicator */
#define UCR3_ADNIMP	(1<<7)  /* Autobaud Detection Not Improved */
#define UCR3_RXDSEN	(1<<6)  /* Receive status interrupt enable */
#define UCR3_AIRINTEN	(1<<5)  /* Async IR wake interrupt enable */
#define UCR3_AWAKEN	(1<<4)  /* Async wake interrupt enable */
#define UCR3_REF25	(1<<3)  /* Ref freq 25 MHz */
#define UCR3_REF30	(1<<2)  /* Ref Freq 30 MHz */

/* imx8 names these bitsfields instead: */
#define UCR3_DTRDEN	BIT(3)  /* bit not used in this chip */
#define UCR3_RXDMUXSEL	BIT(2)  /* RXD muxed input selected; 'should always be set' */

#define UCR3_INVT	(1<<1)  /* Inverted Infrared transmission */
#define UCR3_BPEN	(1<<0)  /* Preset registers enable */
#define UCR4_CTSTL_32	(32<<10) /* CTS trigger level (32 chars) */
#define UCR4_INVR	(1<<9)  /* Inverted infrared reception */
#define UCR4_ENIRI	(1<<8)  /* Serial infrared interrupt enable */
#define UCR4_WKEN	(1<<7)  /* Wake interrupt enable */
#define UCR4_REF16	(1<<6)  /* Ref freq 16 MHz */
#define UCR4_IRSC	(1<<5)  /* IR special case */
#define UCR4_TCEN	(1<<3)  /* Transmit complete interrupt enable */
#define UCR4_BKEN	(1<<2)  /* Break condition interrupt enable */
#define UCR4_OREN	(1<<1)  /* Receiver overrun interrupt enable */
#define UCR4_DREN	(1<<0)  /* Recv data ready interrupt enable */
#define UFCR_RXTL_SHF	0       /* Receiver trigger level shift */
#define UFCR_RFDIV	(7<<7)  /* Reference freq divider mask */
#define UFCR_RFDIV_SHF	7	/* Reference freq divider shift */
#define RFDIV		4	/* divide input clock by 2 */
#define UFCR_DCEDTE	(1<<6)  /* DTE mode select */
#define UFCR_TXTL_SHF	10      /* Transmitter trigger level shift */
#define USR1_PARITYERR	(1<<15) /* Parity error interrupt flag */
#define USR1_RTSS	(1<<14) /* RTS pin status */
#define USR1_TRDY	(1<<13) /* Transmitter ready interrupt/dma flag */
#define USR1_RTSD	(1<<12) /* RTS delta */
#define USR1_ESCF	(1<<11) /* Escape seq interrupt flag */
#define USR1_FRAMERR	(1<<10) /* Frame error interrupt flag */
#define USR1_RRDY	(1<<9)	/* Receiver ready interrupt/dma flag */
#define USR1_TIMEOUT	(1<<7)	/* Receive timeout interrupt status */
#define USR1_RXDS	(1<<6)	/* Receiver idle interrupt flag */
#define USR1_AIRINT	(1<<5)	/* Async IR wake interrupt flag */
#define USR1_AWAKE	(1<<4)	/* Aysnc wake interrupt flag */
#define USR2_ADET	(1<<15) /* Auto baud rate detect complete */
#define USR2_TXFE	(1<<14) /* Transmit buffer FIFO empty */
#define USR2_DTRF	(1<<13) /* DTR edge interrupt flag */
#define USR2_IDLE	(1<<12) /* Idle condition */
#define USR2_IRINT	(1<<8)	/* Serial infrared interrupt flag */
#define USR2_WAKE	(1<<7)	/* Wake */
#define USR2_RTSF	(1<<4)	/* RTS edge interrupt flag */
#define USR2_TXDC	(1<<3)	/* Transmitter complete */
#define USR2_BRCD	(1<<2)	/* Break condition */
#define USR2_ORE	(1<<1)	/* Overrun error */
#define USR2_RDR	(1<<0)	/* Recv data ready */
#define UTS_FRCPERR	(1<<13) /* Force parity error */
#define UTS_LOOP	(1<<12) /* Loop tx and rx */
#define UTS_TXEMPTY	(1<<6)	/* TxFIFO empty */
#define UTS_RXEMPTY	(1<<5)	/* RxFIFO empty */
#define UTS_TXFULL	(1<<4)	/* TxFIFO full */
#define UTS_RXFULL	(1<<3)	/* RxFIFO full */
#define UTS_SOFTRS	(1<<0)	/* Software reset */
#define TXTL		2  /* reset default */
#define RXTL		1  /* reset default */

/* IOMUXC 基地址 */
#define IOMUXC_BASE                     0x020E0000

/* UART1 引脚复用寄存器 */
#define IOMUXC_SW_MUX_CTL_PAD_UART1_TX_DATA     (IOMUXC_BASE + 0x0084)
#define IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA     (IOMUXC_BASE + 0x0088)

/* UART1 引脚电气属性寄存器 */
#define IOMUXC_SW_PAD_CTL_PAD_UART1_TX_DATA     (IOMUXC_BASE + 0x0310)
#define IOMUXC_SW_PAD_CTL_PAD_UART1_RX_DATA     (IOMUXC_BASE + 0x0314)

struct mxc_uart {
	u32 rxd;
	u32 spare0[15];

	u32 txd;
	u32 spare1[15];

	u32 cr1;
	u32 cr2;
	u32 cr3;
	u32 cr4;

	u32 fcr;
	u32 sr1;
	u32 sr2;
	u32 esc;

	u32 tim;
	u32 bir;
	u32 bmr;
	u32 brc;

	u32 onems;
	u32 ts;
};

/*
 * UART1 引脚 PAD 配置值:
 * - HYS(16): 1 - 使能迟滞比较器
 * - PUS(15:14): 00 - 100K 下拉
 * - PUE(13): 0 - Keeper
 * - PKE(12): 1 - 使能 Pull/Keeper
 * - ODE(11): 0 - 禁止开漏
 * - SPEED(7:6): 10 - 100MHz
 * - DSE(5:3): 110 - R0/6 驱动强度
 * - SRE(0): 0 - 慢速摆率
 * 
 * 典型值: 0x10B0
 */
#define UART_PAD_CTRL   0x10B0

static void uart1_iomux_init(void)
{
    /* 配置 UART1_TX_DATA 引脚 */
    /* MUX_MODE = 0: 选择 UART1_TX 功能 */
    writel(0x0, (void *)IOMUXC_SW_MUX_CTL_PAD_UART1_TX_DATA);
    writel(UART_PAD_CTRL, (void *)IOMUXC_SW_PAD_CTL_PAD_UART1_TX_DATA);

    /* 配置 UART1_RX_DATA 引脚 */
    /* MUX_MODE = 0: 选择 UART1_RX 功能 */
    writel(0x0, (void *)IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA);
    writel(UART_PAD_CTRL, (void *)IOMUXC_SW_PAD_CTL_PAD_UART1_RX_DATA);
}

static inline void schedule(void)
{
}

/**
 * @brief   延时函数
 */
static void _delay(volatile u32 count)
{
    while (count--) {
        __asm__ __volatile__("nop");
    }
}

static void _mxc_serial_flush(struct mxc_uart *base)
{
	unsigned int timeout = 4000;

	if (!(readl(&base->cr1) & UCR1_UARTEN) ||
	    !(readl(&base->cr2) & UCR2_TXEN))
		return;

	while (!(readl(&base->sr2) & USR2_TXDC) && --timeout)
		_delay(1000);
}

static void _mxc_serial_init(struct mxc_uart *base, int use_dte)
{
	_mxc_serial_flush(base);

	writel(0, &base->cr1);
	writel(0, &base->cr2);

	while (!(readl(&base->cr2) & UCR2_SRST));

	if (use_dte)
		writel(0x404 | UCR3_ADNIMP, &base->cr3);
	else
		writel(0x704 | UCR3_ADNIMP, &base->cr3);

	writel(0x704 | UCR3_ADNIMP, &base->cr3);
	writel(0x8000, &base->cr4);
	writel(0x2b, &base->esc);
	writel(0, &base->tim);

	writel(0, &base->ts);
}

static void _mxc_serial_setbrg(struct mxc_uart *base, unsigned long clk,
			       unsigned long baudrate, bool use_dte)
{
	u32 tmp;

	_mxc_serial_flush(base);

	tmp = RFDIV << UFCR_RFDIV_SHF;
	if (use_dte)
		tmp |= UFCR_DCEDTE;
	else
		tmp |= (TXTL << UFCR_TXTL_SHF) | (RXTL << UFCR_RXTL_SHF);
	writel(tmp, &base->fcr);

	writel(0xf, &base->bir);
	writel(clk / (2 * baudrate), &base->bmr);

	writel(UCR2_WS | UCR2_IRTS | UCR2_RXEN | UCR2_TXEN | UCR2_SRST,
	       &base->cr2);

	/*
	 * setting the baudrate triggers a reset, returning cr3 to its
	 * reset value but UCR3_RXDMUXSEL "should always be set."
	 * according to the imx8 reference-manual
	 */
	writel(readl(&base->cr3) | UCR3_RXDMUXSEL, &base->cr3);

	writel(UCR1_UARTEN, &base->cr1);
}

#ifdef CONFIG_DEBUG_UART_MXC
#include <debug_uart.h>

static inline void _debug_uart_init(void)
{
	struct mxc_uart *base = (struct mxc_uart *)CONFIG_VAL(DEBUG_UART_BASE);

    uart1_iomux_init();

	_mxc_serial_init(base, false);
	_mxc_serial_setbrg(base, CONFIG_DEBUG_UART_CLOCK,
			   CONFIG_BAUDRATE, false);
}

static inline void _debug_uart_putc(int ch)
{
	struct mxc_uart *base = (struct mxc_uart *)CONFIG_VAL(DEBUG_UART_BASE);

	while (!(readl(&base->ts) & UTS_TXEMPTY))
		schedule();

	writel(ch, &base->txd);
}

DEBUG_UART_FUNCS

#endif
