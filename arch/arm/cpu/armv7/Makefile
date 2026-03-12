# SPDX-License-Identifier: GPL-2.0+
#
# (C) Copyright 2000-2003
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.

extra-y	:= start.o

ifneq ($(CONFIG_$(XPL_)SKIP_LOWLEVEL_INIT),y)
obj-y	+= lowlevel_init.o
endif