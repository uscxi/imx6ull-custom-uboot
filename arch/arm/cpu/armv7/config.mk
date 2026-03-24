# SPDX-License-Identifier: GPL-2.0+
#
# (C) Copyright 2002
# Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>

# On supported platforms we set the bit which causes us to trap on unaligned
# memory access.  This is the opposite of what the compiler expects to be
# the default so we must pass in -mno-unaligned-access so that it is aware
# of our decision.
# 测试当前编译器能否识别并使用第一个参数提供的选项（-mno-unaligned-access）。
# 如果能，宏就返回该选项；如果不能，则返回第二个参数（此处为空）。
# -mno-unaligned-access选项的作用是指示编译器，生成的代码应当避免使用非对齐的内存访问
PF_NO_UNALIGNED := $(call cc-option, -mno-unaligned-access,)
PLATFORM_CPPFLAGS += $(PF_NO_UNALIGNED)
