# SPDX-License-Identifier: GPL-2.0+

obj-y += initcall.o
obj-y += hang.o
obj-y += string.o
obj-$(CONFIG_$(PHASE_)OF_LIBFDT) += libfdt/
obj-$(CONFIG_$(PHASE_)OF_REAL) += fdtdec.o
#obj-$(CONFIG_DEBUG_UART) += early_printf.o
obj-y += panic.o
obj-y += vsprintf.o
obj-y += display_options.o
CFLAGS_display_options.o := $(if $(BUILD_TAG),-DBUILD_TAG='"$(BUILD_TAG)"')
obj-y += ctype.o
