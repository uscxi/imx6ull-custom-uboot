/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Early debug UART support
 */

#ifndef _DEBUG_UART_H
#define _DEBUG_UART_H

/*
 * 调试UART旨在用于U-Boot启动的极早期阶段，以便在ICE(In-Circuit Emulator,在线仿真器)或其他调试机制不可用时调试问题。
 *
 * 要使用调试UART，您应该：
 * - 确保您的UART硬件支持此接口
 * - 启用配置宏 CONFIG_DEBUG_UART
 * - 启用对应UART的配置宏以告知其提供此接口
 *      （例如 CONFIG_DEBUG_UART_NS16550）
 * - 根据需要定义所需的设置参数（详见下文）
 * - 使用前调用 debug_uart_init() 函数
 * - 调用 printch() 函数输出字符
 *
 * 根据您的硬件平台，可能在栈(stack)尚未可用时即可使用此UART。
 *
 * 如果您的UART硬件不支持此接口，您通常可以较容易地添加支持。
 * 请注意，您不能使用驱动模型(driver model)，且最好避免使用栈。
 *
 * 一旦驱动模型开始工作且串口驱动初始化完成（在 serial_init() 中完成），
 * 就不得再使用此调试UART。否则驱动程序可能会产生冲突，导致输出异常。
 *
 *
 * 要在您的串口驱动中启用调试UART，请按以下步骤操作：
 *
 * - 包含头文件 <debug_uart.h>
 * - 定义 _debug_uart_init() 函数，尽量避免使用栈
 * - 将 _debug_uart_putc() 定义为静态内联函数（避免使用栈）
 * - 紧接着添加 DEBUG_UART_FUNCS 宏以定义其余功能
 *     （如 printch() 等函数）
 *
 * 如果您的硬件板需要额外初始化才能使UART正常工作，
 * 请启用 CONFIG_DEBUG_UART_BOARD_INIT 配置宏，
 * 并编写名为 board_debug_uart_init() 的函数来执行该初始化。
 * 当调用 debug_uart_init() 时，此初始化将自动执行。
 */

/**
 * debug_uart_init() - 设置调试UART为就绪状态
 *
 * 此函数将正确设置UART的波特率等参数。
 *
 * 可用的配置宏包括：
 *
 *    - CONFIG_DEBUG_UART_BASE: UART的基地址
 *    - CONFIG_BAUDRATE: 请求的波特率
 *    - CONFIG_DEBUG_UART_CLOCK: UART的输入时钟频率
 */

void debug_uart_init(void);

/**
 * printch() - Output a character to the debug UART
 *
 * @ch:		Character to output
 */
void printch(int ch);

/**
 * printascii() - Output an ASCII string to the debug UART
 *
 * @str:	String to output
 */
void printascii(const char *str);

/**
 * printhex2() - Output a 2-digit hex value
 *
 * @value:	Value to output
 */
void printhex2(unsigned int value);

/**
 * printhex4() - Output a 4-digit hex value
 *
 * @value:	Value to output
 */
void printhex4(unsigned int value);

/**
 * printhex8() - Output a 8-digit hex value
 *
 * @value:	Value to output
 */
void printhex8(unsigned int value);

/**
 * printdec() - Output a decimalism value
 *
 * @value:	Value to output
 */
void printdec(unsigned int value);

#ifdef CONFIG_DEBUG_UART_ANNOUNCE
#define _DEBUG_UART_ANNOUNCE	printascii("\n<debug_uart>\n");
#else
#define _DEBUG_UART_ANNOUNCE
#endif

/*
 * Now define some functions - this should be inserted into the serial driver
 */
#define DEBUG_UART_FUNCS \
\
	static inline void _printch(int ch) \
	{ \
		if (ch == '\n') \
			_debug_uart_putc('\r'); \
		_debug_uart_putc(ch); \
	} \
\
	void printch(int ch) \
	{ \
		_printch(ch); \
	} \
\
	void printascii(const char *str) \
	{ \
		while (*str) \
			_printch(*str++); \
	} \
\
	static inline void printhex1(unsigned int digit) \
	{ \
		digit &= 0xf; \
		_debug_uart_putc(digit > 9 ? digit - 10 + 'a' : digit + '0'); \
	} \
\
	static inline void printhex(unsigned int value, int digits) \
	{ \
		while (digits-- > 0) \
			printhex1(value >> (4 * digits)); \
	} \
\
	void printhex2(unsigned int value) \
	{ \
		printhex(value, 2); \
	} \
\
	void printhex4(unsigned int value) \
	{ \
		printhex(value, 4); \
	} \
\
	void printhex8(unsigned int value) \
	{ \
		printhex(value, 8); \
	} \
\
	void printdec(unsigned int value) \
	{ \
		if (value > 10) { \
			printdec(value / 10); \
			value %= 10; \
		} else if (value == 10) { \
			_debug_uart_putc('1'); \
			value = 0; \
		} \
		_debug_uart_putc('0' + value); \
	} \
\
	void debug_uart_init(void) \
	{ \
		_debug_uart_init(); \
		_DEBUG_UART_ANNOUNCE \
	} \

#endif
