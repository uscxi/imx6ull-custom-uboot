#ifndef __FDTDEC_H
#define __FDTDEC_H

#include <linux/libfdt.h>
#include <linux/types.h>


/*
 * Support for 64bit fdt addresses.
 * This can be used not only for 64bit SoCs, but also
 * for large address extensions on 32bit SoCs.
 * Note that fdt data is always big
 * endian even on a litle endian machine.
 */

#define FDT_SIZE_T_NONE (-1U)

#ifdef CONFIG_FDT_64BIT
typedef u64 fdt_addr_t;
typedef u64 fdt_size_t;
#define FDT_ADDR_T_NONE ((ulong)(-1))

#define fdt_addr_to_cpu(reg) be64_to_cpu(reg)
#define fdt_size_to_cpu(reg) be64_to_cpu(reg)
#define cpu_to_fdt_addr(reg) cpu_to_be64(reg)
#define cpu_to_fdt_size(reg) cpu_to_be64(reg)
typedef fdt64_t fdt_val_t;
#else
typedef u32 fdt_addr_t;
typedef u32 fdt_size_t;
#define FDT_ADDR_T_NONE (-1U)

#define fdt_addr_to_cpu(reg) be32_to_cpu(reg)
#define fdt_size_to_cpu(reg) be32_to_cpu(reg)
#define cpu_to_fdt_addr(reg) cpu_to_be32(reg)
#define cpu_to_fdt_size(reg) cpu_to_be32(reg)
typedef fdt32_t fdt_val_t;
#endif

/**
 * enum fdt_source_t - indicates where the devicetree came from
 *
 * These are listed in approximate order of desirability after FDTSRC_NONE
 *
 * @FDTSRC_SEPARATE: Appended to U-Boot. This is the normal approach if U-Boot
 *	is the only firmware being booted
 * @FDTSRC_FIT: Found in a multi-dtb FIT. This should be used when U-Boot must
 *	select a devicetree from many options
 * @FDTSRC_BOARD: Located by custom board code. This should only be used when
 *	the prior stage does not support FDTSRC_PASSAGE
 * @FDTSRC_EMBED: Embedded into U-Boot executable. This should onyl be used when
 *	U-Boot is packaged as an ELF file, e.g. for debugging purposes
 * @FDTSRC_ENV: Provided by the fdtcontroladdr environment variable. This should
 *	be used for debugging/development only
 * @FDTSRC_BLOBLIST: Provided by a bloblist from an earlier phase
 */
enum fdt_source_t {
	FDTSRC_SEPARATE,
	FDTSRC_FIT,
	FDTSRC_BOARD,
	FDTSRC_EMBED,
	FDTSRC_ENV,
	FDTSRC_BLOBLIST,
};

/**
 * Set up the device tree ready for use
 */
int fdtdec_setup(void);

#endif // __FDTDEC_H
