

#include <debug_uart.h>
#include <display_options.h>
#include <stdarg.h>
#include <serial.h>
#include <asm/global_data.h>

DECLARE_GLOBAL_DATA_PTR;

void putc(const char c)
{
	if (!gd)
		return;

	/* if we don't have a console yet, use the debug UART */
	if (IS_ENABLED(CONFIG_DEBUG_UART) && !(gd->flags & GD_FLG_SERIAL_READY)) {
		printch(c);
		return;
	}
}

void puts(const char *s)
{
	if (!gd)
		return;

	if (IS_ENABLED(CONFIG_DEBUG_UART) && !(gd->flags & GD_FLG_SERIAL_READY)) {
		printascii(s);
		return;
	}
}

#ifdef CONFIG_CONSOLE_FLUSH_SUPPORT
void flush(void)
{
	if (!gd)
		return;

	if (IS_ENABLED(CONFIG_DEBUG_UART) && !(gd->flags & GD_FLG_SERIAL_READY))
		return;
}
#endif
