# SPDX-License-Identifier: GPL-2.0
###
# Makefile.basic 列出了构建过程中使用的基本程序。
# 此处列出的程序用于完成基础操作，例如修复文件依赖关系。
# 此初始步骤旨在避免内核配置变更时（当主 Makefile 包含 .config 文件时即会发生此情况）文件被重新编译。
# ---------------------------------------------------------------------------
# fixdep:     用于在构建过程中生成依赖关系信息

hostprogs-y := fixdep
always		:= $(hostprogs-y)

# fixdep 是编译其他主机程序所必需的
$(addprefix $(obj)/,$(filter-out fixdep,$(always))): $(obj)/fixdep
