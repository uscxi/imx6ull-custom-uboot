#include <display_options.h>
#include <hang.h>
#include <init.h>
#include <log.h>
#include <errno.h>
#include <fdtdec.h>
#include <linux/libfdt.h>
#include <asm/global_data.h>
#include <asm/sections.h>

DECLARE_GLOBAL_DATA_PTR;

extern u8 __dtb_dt_begin[];	/* embedded device tree blob */
extern u8 __dtb_dt_spl_begin[];	/* embedded device tree blob for SPL/TPL */

/* Get a pointer to the embedded devicetree, if there is one, else NULL */
static u8 *dtb_dt_embedded(void)
{
	u8 *addr = NULL;

	if (IS_ENABLED(CONFIG_OF_EMBED)) {
		addr = __dtb_dt_begin;

		if (IS_ENABLED(CONFIG_XPL_BUILD))
			addr = __dtb_dt_spl_begin;
	}
	return addr;
}

/**
 * fdtdec_prepare_fdt() - Check we have a valid fdt available to control U-Boot
 *
 * @blob: Blob to check
 *
 * If not, a message is printed to the console if the console is ready.
 *
 * Return: 0 if all ok, -ENOENT if not
 */
static int fdtdec_prepare_fdt(const void *blob)
{
	if (!blob || ((uintptr_t)blob & 3) || fdt_check_header(blob)) {
		printf("No valid device tree binary found at %p\n",
			    blob);
		if (_DEBUG && blob) {
			printf("fdt_blob=%p\n", blob);
			print_buffer((ulong)blob, blob, 4, 32, 0);
		}
	    return -ENOENT;
	}

	return 0;
}

/**
 * fdt_find_separate() - Find a devicetree at the end of the image
 *
 * Return: pointer to FDT blob
 */
static void *fdt_find_separate(void)
{
	void *fdt_blob = NULL;

    /* FDT is at end of image */
	fdt_blob = (ulong *)_end;

    return fdt_blob;
}

/* TODO(sjg@chromium.org): This function should not be weak */
__weak int fdtdec_board_setup(const void *fdt_blob)
{
	return 0;
}

void fdtdec_setup_embed(void)
{
	gd->fdt_blob = dtb_dt_embedded();
	gd->fdt_src = FDTSRC_EMBED;
}

int fdtdec_setup(void)
{
	int ret = -ENOENT;

    /* The devicetree is typically appended to U-Boot */
	if (ret) {
		if (IS_ENABLED(CONFIG_OF_SEPARATE)) {
			gd->fdt_blob = fdt_find_separate();
			gd->fdt_src = FDTSRC_SEPARATE;
		} else { /* embed dtb in ELF file for testing / development */
			fdtdec_setup_embed();
		}
	}

    ret = fdtdec_prepare_fdt(gd->fdt_blob);
	if (!ret)
		ret = fdtdec_board_setup(gd->fdt_blob);

    return ret;
}
