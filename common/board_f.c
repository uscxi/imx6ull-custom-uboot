#include <config.h>
#include <initcall.h>
#include <init.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/sections.h>
#include <hang.h>
#include <log.h>
#include <fdtdec.h>

DECLARE_GLOBAL_DATA_PTR;


static int setup_mon_len(void)
{
#if defined(__ARM__)
    gd->mon_len = (ulong)__bss_end - (ulong)_start;
#endif
    return 0;
}

static const init_fnc_t init_sequence_f[] = {
	setup_mon_len,
#ifdef CONFIG_OF_CONTROL
	fdtdec_setup,
#endif
    NULL,
};

void board_init_f(ulong boot_flags)
{
	// struct board_f boardf;

	gd->flags = boot_flags;
	gd->flags &= ~GD_FLG_HAVE_CONSOLE;
	// gd->boardf = &boardf;

    if (initcall_run_list(init_sequence_f))
		hang();

	main_loop();
}
