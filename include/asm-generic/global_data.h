/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2012 The Chromium OS Authors.
 * (C) Copyright 2002-2010
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

#ifndef __ASM_GENERIC_GBL_DATA_H
#define __ASM_GENERIC_GBL_DATA_H
/*
 * The following data structure is placed in some memory which is
 * available very early after boot (like DPRAM on MPC8xx/MPC82xx, or
 * some locked parts of the data cache) to allow for a minimum set of
 * global variables during system initialization (until we have set
 * up the memory controller so that we can use RAM).
 *
 * Keep it *SMALL* and remember to set GENERATED_GBL_DATA_SIZE > sizeof(gd_t)
 *
 * Each architecture has its own private fields. For now all are private
 */

#ifndef __ASSEMBLY__
#include <asm-offsets.h>
#include <fdtdec.h>
#include <linux/build_bug.h>

typedef struct global_data gd_t;

/**
 * struct global_data - global data structure
 */
struct global_data {
    /**
	 * @bd: board information
	 */
	struct bd_info *bd;
	/**
	 * @new_gd: pointer to relocated global data
	 */
	struct global_data *new_gd;
    /**
	 * @fdt_blob: U-Boot's own device tree, NULL if none
	 */
	const void *fdt_blob;

    /**
	 * @fdt_src: Source of FDT
	 */
	enum fdt_source_t fdt_src;
// 	/**
// 	 * @cur_serial_dev: current serial device
// 	 */
// 	struct udevice *cur_serial_dev;
// #ifndef CONFIG_XPL_BUILD
// 	/**
// 	 * @jt: jump table
// 	 *
// 	 * The jump table contains pointers to exported functions. A pointer to
// 	 * the jump table is passed to standalone applications.
// 	 */
// 	struct jt_funcs *jt;
// 	/**
// 	 * @boardf: information only used before relocation
// 	 */
// 	struct board_f *boardf;
// #endif
    /**
	 * @ram_size: RAM size in bytes
	 */
	phys_size_t ram_size;
	/**
	 * @ram_top: top address of RAM used by U-Boot
	 */
	phys_addr_t ram_top;
	/**
	 * @flags: global data flags
	 *
	 * See &enum gd_flags
	 */
	unsigned long flags;
	/**
	 * @cpu_clk: CPU clock rate in Hz
	 */
	unsigned long cpu_clk;
    /**
	 * @ram_base: base address of RAM used by U-Boot
	 */
	unsigned long ram_base;
	/**
	 * @relocaddr: start address of U-Boot in RAM
	 *
	 * After relocation this field indicates the address to which U-Boot
	 * has been relocated. It can be displayed using the bdinfo command.
	 * Its value is needed to display the source code when debugging with
	 * GDB using the 'add-symbol-file u-boot <relocaddr>' command.
	 */
	unsigned long relocaddr;
	/**
	 * @irq_sp: IRQ stack pointer
	 */
	unsigned long irq_sp;
	/**
	 * @start_addr_sp: initial stack pointer address
	 */
	unsigned long start_addr_sp;
	/**
	 * @reloc_off: relocation offset
	 */
	unsigned long reloc_off;
	/**
	 * @bus_clk: platform clock rate in Hz
	 */
	unsigned int bus_clk;
	/**
	 * @mem_clk: memory clock rate in Hz
	 */
	unsigned int mem_clk;
	/**
	 * @mon_len: monitor length in bytes
	 */
	unsigned int mon_len;
	/**
	 * @baudrate: baud rate of the serial interface
	 */
	unsigned int baudrate;
#if CONFIG_IS_ENABLED(SYS_MALLOC_F)
	/**
	 * @malloc_base: base address of early malloc()
	 */
	unsigned long malloc_base;
	/**
	 * @malloc_limit: maximum size of early malloc()
	 */
	unsigned int malloc_limit;
	/**
	 * @malloc_ptr: currently used bytes of early malloc()
	 */
	unsigned int malloc_ptr;
#endif
};
#ifndef DO_DEPS_ONLY
static_assert(sizeof(struct global_data) == GD_SIZE);
#endif

/**
 * enum gd_flags - global data flags
 *
 * See field flags of &struct global_data.
 */
enum gd_flags {
	/**
	 * @GD_FLG_RELOC: code was relocated to RAM
	 */
	GD_FLG_RELOC = 0x00001,
	/**
	 * @GD_FLG_DEVINIT: devices have been initialized
	 */
	GD_FLG_DEVINIT = 0x00002,
    /**
	 * @GD_FLG_SERIAL_READY: pre-relocation serial console ready
	 */
	GD_FLG_SERIAL_READY = 0x00100,
    /**
	 * @GD_FLG_HAVE_CONSOLE: serial_init() was called and a console
	 * is available. When not set, indicates that console input and output
	 * drivers shall not be called.
	 */
	GD_FLG_HAVE_CONSOLE = 0x8000000,
};

#endif /* __ASSEMBLY__ */

#endif /* __ASM_GENERIC_GBL_DATA_H */
