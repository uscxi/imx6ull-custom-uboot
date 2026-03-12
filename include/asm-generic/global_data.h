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

typedef struct global_data gd_t;

/**
 * struct global_data - global data structure
 */
struct global_data {
	/**
	 * @new_gd: pointer to relocated global data
	 */
	struct global_data *new_gd;
};
#ifndef DO_DEPS_ONLY
static_assert(sizeof(struct global_data) == GD_SIZE);
#endif

#endif /* __ASSEMBLY__ */

#endif /* __ASM_GENERIC_GBL_DATA_H */
