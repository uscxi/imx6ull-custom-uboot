#include <initcall.h>
#include <log.h>
#include <asm/global_data.h>

DECLARE_GLOBAL_DATA_PTR;

static ulong calc_reloc_ofs(void)
{
	/*
	 * Sandbox is relocated by the OS, so symbols always appear at
	 * the relocated address.
	 */
	if (gd->flags & GD_FLG_RELOC)
		return gd->reloc_off;

	return 0;
}

/*
 * To enable debugging. add #define DEBUG at the top of the including file.
 *
 * To find a symbol, use grep on u-boot.map
 */
int initcall_run_list(const init_fnc_t init_sequence[])
{
	ulong reloc_ofs;
	const init_fnc_t *ptr;
	init_fnc_t func;
	int ret = 0;

	for (ptr = init_sequence; func = *ptr, func; ptr++) {
		reloc_ofs = calc_reloc_ofs();

		if (reloc_ofs) {
			debug("initcall: %p (relocated to %p)\n",
			      (char *)func - reloc_ofs, (char *)func);
		} else {
			debug("initcall: %p\n", (char *)func - reloc_ofs);
		}

		ret = func();
		if (ret)
			break;
	}

	if (ret) {
		debug("initcall failed at call %p (err=%d)\n",
			(char *)func - reloc_ofs, ret);

		return ret;
	}

	return 0;
}
