#ifndef __LOG_H
#define __LOG_H

#include <stdio.h>

/* Define this at the top of a file to add a prefix to debug messages */
#ifndef pr_fmt
#define pr_fmt(fmt) fmt
#endif

#if CONFIG_IS_ENABLED(DEBUG_UART)
#ifndef DEBUG
#define DEBUG
#endif
#endif

#ifdef DEBUG
#define _DEBUG	1
#else
#define _DEBUG	0
#endif

void early_printf(const char *fmt, ...);

#if CONFIG_IS_ENABLED(DEBUG_UART)
#define debug_cond(cond, fmt, args...)		\
({						\
	if (cond)				\
		printf(pr_fmt(fmt), ##args);	\
})
#endif

/* Show a message if DEBUG is defined in a file */
#define debug(fmt, args...)			\
	debug_cond(_DEBUG, fmt, ##args)

/*
 * An assertion is run-time check done in debug mode only. If DEBUG is not
 * defined then it is skipped. If DEBUG is defined and the assertion fails,
 * then it calls panic*( which may or may not reset/halt U-Boot (see
 * CONFIG_PANIC_HANG), It is hoped that all failing assertions are found
 * before release, and after release it is hoped that they don't matter. But
 * in any case these failing assertions cannot be fixed with a reset (which
 * may just do the same assertion again).
 */
void __assert_fail(const char *assertion, const char *file, unsigned int line,
		   const char *function);

/**
 * assert() - assert expression is true
 *
 * If the expression x evaluates to false and _DEBUG evaluates to true, a panic
 * message is written and the system stalls. The value of _DEBUG is set to true
 * if DEBUG is defined before including common.h.
 *
 * The expression x is always executed irrespective of the value of _DEBUG.
 *
 * @x:		expression to test
 */
#define assert(x) \
	({ if (!(x) && _DEBUG) \
		__assert_fail(#x, __FILE__, __LINE__, __func__); })

#endif
