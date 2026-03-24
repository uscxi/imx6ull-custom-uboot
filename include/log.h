#ifndef __LOG_H
#define __LOG_H

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
		early_printf(pr_fmt(fmt), ##args);	\
})
#endif

/* Show a message if DEBUG is defined in a file */
#define debug(fmt, args...)			\
	debug_cond(_DEBUG, fmt, ##args)

#endif
