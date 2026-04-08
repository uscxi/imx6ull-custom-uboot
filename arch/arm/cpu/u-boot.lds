/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2004-2008 Texas Instruments
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
 */

#include <config.h>

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS{
	. = 0x00000000;

	. = ALIGN(4);
	.text :
	{
		*(.vectors)
		CPUDIR/start.o (.text*)
	}

    .text_rest :
	{
		*(.text*)
	}

    . = ALIGN(4);
	.rodata : {*(.rodata*)}

    . = ALIGN(4);
	.data : {
        *(.data)
    }
    
    . = ALIGN(4);

    _end = .;
    _image_binary_end = .;

	.bss :
    {
        __bss_start = .;
		*(.bss*)
		. = ALIGN(4);
		__bss_end = .;
    }
}
