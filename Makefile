# 版本信息：<主版本>.<次版本>.<补丁版本>...
# VERSION              主版本号，通常与发布年份对应
# PATCHLEVEL   次版本号，通常与发布月份对应（例如04代表四月）
# SUBLEVEL             补丁级别，表示当前版本的第几次修正（此处为空，表示无额外补丁）
# EXTRAVERSION 额外版本标识，用于标记候选版本（如 -rc1、-rc2）或自定义后缀
# NAME                 版本代号，可填写该版本的昵称，通常用于开发者内部标识
VERSION = 2025
PATCHLEVEL = 04
SUBLEVEL =
EXTRAVERSION =
NAME =


# 禁用内置规则和内置变量
# 强制开发者写出所有规则，使得 Makefile 更加自包含，便于理解和维护。
MAKEFLAGS += -rR

# Determine target architecture for the sandbox
include include/host_arch.h
ifeq ("", "$(CROSS_COMPILE)")
  MK_ARCH="${shell uname -m}"
else
  MK_ARCH="${shell echo $(CROSS_COMPILE) | sed -n 's/^\(.*ccache\)\{0,1\}[[:space:]]*\([^\/]*\/\)*\([^-]*\)-[^[:space:]]*/\3/p'}"
endif
unexport HOST_ARCH
ifeq ("x86_64", $(MK_ARCH))
  export HOST_ARCH=$(HOST_ARCH_X86_64)
else ifneq (,$(findstring $(MK_ARCH), "aarch64" "armv8l"))
  export HOST_ARCH=$(HOST_ARCH_AARCH64)
else ifneq (,$(findstring $(MK_ARCH), "arm" "armv7" "armv7a" "armv7l"))
  export HOST_ARCH=$(HOST_ARCH_ARM)
endif
undefine MK_ARCH

# 输出美化
ifeq ("$(origin V)", "command line")
  KBUILD_VERBOSE = $(V)
endif
ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE = 0
endif

ifeq ($(KBUILD_VERBOSE),1)
  quiet =
  Q =
else
  quiet=quiet_
  Q = @
endif

# 检测 make -s（静默模式）的兼容性处理
# make 4.x 和 3.8x 在 MAKEFLAGS 中表示 -s 的方式不同：
#   make 4.x: MAKEFLAGS 第一个词中包含 's'，且前面可能有其他单字符选项
#   make 3.8x: MAKEFLAGS 以 "s" 开头或包含 "-s"
ifneq ($(filter 4.%,$(MAKE_VERSION)),)	# make-4
ifneq ($(filter %s ,$(firstword x$(MAKEFLAGS))),)
  quiet=silent_
endif
else					# make-3.8x
ifneq ($(filter s% -s%,$(MAKEFLAGS)),)
  quiet=silent_
endif
endif

export quiet Q KBUILD_VERBOSE

ifeq ($(KBUILD_SRC),)

ifeq ("$(origin O)", "command line")
  KBUILD_OUTPUT := $(O)
endif

# That's our default target when none is given on the command line
PHONY := _all
_all:

# Cancel implicit rules on top Makefile
$(CURDIR)/Makefile Makefile: ;

ifneq ($(KBUILD_OUTPUT),)
# Invoke a second make in the output directory, passing relevant variables
# check that the output directory actually exists
saved-output := $(KBUILD_OUTPUT)
KBUILD_OUTPUT := $(shell mkdir -p $(KBUILD_OUTPUT) && cd $(KBUILD_OUTPUT) \
								&& /bin/pwd)
$(if $(KBUILD_OUTPUT),, \
     $(error failed to create output directory "$(saved-output)"))

# Look for make include files relative to root of kernel src
#
# This does not become effective immediately because MAKEFLAGS is re-parsed
# once after the Makefile is read.  It is OK since we are going to invoke
# 'sub-make' below.
MAKEFLAGS += --include-dir=$(CURDIR)

PHONY += $(MAKECMDGOALS) sub-make

$(filter-out _all sub-make $(CURDIR)/Makefile, $(MAKECMDGOALS)) _all: sub-make
	@:

sub-make: FORCE
	$(Q)$(MAKE) -C $(KBUILD_OUTPUT) KBUILD_SRC=$(CURDIR) \
	-f $(CURDIR)/Makefile $(filter-out _all sub-make,$(MAKECMDGOALS))

# Leave processing to above invocation of make
skip-makefile := 1
endif # ifneq ($(KBUILD_OUTPUT),)
endif # ifeq ($(KBUILD_SRC),)

# 只有当 skip-makefile 未设置时，才继续处理后续内容。
# 此时我们要么在源码目录中直接构建（无 O=），
# 要么在输出目录中作为 sub-make 运行。
ifeq ($(skip-makefile),)

MAKEFLAGS += --no-print-directory

PHONY += all
_all: all

ifeq ($(KBUILD_SRC),)
    # building in the source tree
    srctree := .
else
    ifeq ($(KBUILD_SRC)/,$(dir $(CURDIR)))
        # building in a subdirectory of the source tree
        srctree := ..
    else
        srctree := $(KBUILD_SRC)
    endif
endif
# 构建产物总是在当前目录
objtree	:= .
src		:= $(srctree)
obj		:= $(objtree)

# 当 make 在当前目录找不到依赖文件时，会自动到 VPATH 指定的目录中查找
VPATH	:= $(srctree)

export srctree objtree VPATH

# CDPATH 是 bash 的环境变量，会影响 cd 命令的行为
# 在 Makefile 中执行 shell 命令时，CDPATH 可能导致 cd 跳转到意外目录
unexport CDPATH

# 探测宿主机 CPU 架构，统一名称。
HOSTARCH := $(shell uname -m | \
	sed -e s/i.86/x86/ \
	    -e s/sun4u/sparc64/ \
	    -e s/arm.*/arm/ \
	    -e s/sa110/arm/ \
	    -e s/ppc64/powerpc/ \
	    -e s/ppc/powerpc/ \
	    -e s/macppc/powerpc/\
	    -e s/sh.*/sh/)
# 探测宿主机操作系统
HOSTOS := $(shell uname -s | tr '[:upper:]' '[:lower:]' | \
	    sed -e 's/\(cygwin\).*/cygwin/')

export	HOSTARCH HOSTOS

# ?= 表示仅在变量未定义时才赋值，允许用户覆盖
ifeq ($(HOSTARCH),$(ARCH))
CROSS_COMPILE ?=
endif

# .config 文件路径，Kconfig 系统的核心输出
# 用户可以通过 KCONFIG_CONFIG 环境变量指定替代路径
# 默认为源码根目录下的 .config
KCONFIG_CONFIG	?= .config
export KCONFIG_CONFIG

# 宿主机工具链配置
# CONFIG_SHELL：Kbuild 使用的 shell 解释器
# 优先使用 bash（因为部分 Kbuild 脚本依赖 bash 特性），
# 降级为 /bin/bash，最终降级为 sh
CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	  else if [ -x /bin/bash ]; then echo /bin/bash; \
	  else echo sh; fi ; fi)

# 宿主机编译器（用于编译构建过程中在宿主机上运行的工具）
# 注意区分：
#   HOSTCC     — 编译宿主机工具（如 scripts/kconfig/mconf）
#   CC         — 编译目标平台代码（U-Boot 本身）
HOSTCC       = cc
HOSTCXX      = c++

# 宿主机编译标志
# -Wall: 开启所有常见警告
# -Wstrict-prototypes: 要求函数原型声明中必须有参数列表
# -O2: 开启优化（宿主机工具不需要调试，追求速度）
# -fomit-frame-pointer: 省略帧指针，释放一个寄存器
KBUILD_HOSTCFLAGS   := -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer \
		$(HOSTCFLAGS)
KBUILD_HOSTCXXFLAGS := -O2 $(HOSTCXXFLAGS)
KBUILD_HOSTLDFLAGS  := $(HOSTLDFLAGS)
KBUILD_HOSTLDLIBS   := $(HOSTLDLIBS)

# C 语言标准：使用 GNU C11（C11 + GNU 扩展）
# GNU 扩展包括：内联汇编、语句表达式、typeof、__attribute__ 等
CSTD_FLAG := -std=gnu11
KBUILD_HOSTCFLAGS += $(CSTD_FLAG)

export KBUILD_SRC

# 空规则 "scripts/Kbuild.include: ;" 防止 make 尝试重新生成此文件
scripts/Kbuild.include: ;
include scripts/Kbuild.include

# 目标平台工具链定义
# $(CROSS_COMPILE) 是交叉编译器前缀，例如：
#   arm-linux-gnueabihf-  → AS = arm-linux-gnueabihf-as
#   aarch64-none-elf-     → CC = aarch64-none-elf-gcc

AS		= $(CROSS_COMPILE)as

# 链接器：优先使用 ld.bfd
# 原因：
#   1) BFD (Binary File Descriptor) 链接器是 GNU ld 的传统实现
#   2) gold 链接器虽然更快，但对某些嵌入式链接脚本的支持不完整
#   3) LLVM 的 lld 在裸机（bare-metal）场景下可能有兼容性问题
#   4) U-Boot 的链接脚本依赖 BFD ld 的某些特定行为
ifneq ($(shell $(CROSS_COMPILE)ld.bfd -v 2> /dev/null),)
LD		= $(CROSS_COMPILE)ld.bfd
else
LD		= $(CROSS_COMPILE)ld
endif

CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
LDR		= $(CROSS_COMPILE)ldr
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump
LEX		= flex
YACC		= bison

KBUILD_CPPFLAGS := -D__KERNEL__ -D__UBOOT__

# C 编译器标志
# -Wall:              开启大部分编译警告
# -Wstrict-prototypes: 要求严格的函数原型声明
# -Wno-format-security: 抑制 printf 格式字符串安全警告
#                       （U-Boot 中大量使用运行时动态格式字符串）
# -fno-builtin:       禁用 GCC 内建函数（如 memcpy, printf 的优化替换）
#                     U-Boot 有自己的实现，不能被编译器替换
# -ffreestanding:     告知编译器这是独立环境（非宿主环境）
#                     不假设标准库和启动代码的存在
# -fshort-wchar:      wchar_t 使用 2 字节（与 UEFI 规范一致）
# -fno-strict-aliasing: 禁用严格别名规则优化
#                       嵌入式代码中经常需要通过不同类型指针访问同一内存
KBUILD_CFLAGS   := -Wall -Wstrict-prototypes \
		   -Wno-format-security \
		   -fno-builtin -ffreestanding $(CSTD_FLAG)
KBUILD_CFLAGS	+= -fshort-wchar -fno-strict-aliasing

# 汇编器标志
# -D__ASSEMBLY__: 在汇编文件中定义此宏
#   使得 C 头文件在被汇编文件 include 时可以条件编译掉 C 专有内容
KBUILD_AFLAGS   := -D__ASSEMBLY__

# 链接器标志（此处为空，由架构 Makefile 和 board 配置追加）
KBUILD_LDFLAGS  :=

# 禁止生成位置无关代码（PIE - Position Independent Executable）
# 原因：
#   1) U-Boot 有自己的重定位机制，不依赖 PIE
#   2) PIE 会引入 GOT/PLT 等开销，不适合 bootloader
#   3) 某些现代 GCC 默认开启 PIE，需要显式关闭
# cc-option 是 Kbuild.include 中定义的探测函数：
#   如果编译器支持 -fno-PIE 则使用之，否则忽略
KBUILD_CFLAGS	+= $(call cc-option,-fno-PIE)
KBUILD_AFLAGS	+= $(call cc-option,-fno-PIE)

# 调试信息
DEBUG ?= 0

ifeq ($(DEBUG), 1)
KBUILD_CFLAGS += -g
KBUILD_AFLAGS += -g
endif

# 版本字符串生成
# UBOOTRELEASE: 完整版本字符串，从 include/config/uboot.release 读取
#   该文件由 scripts/setlocalversion 生成
#   内容示例: "2025.04-rc3-00001-gabcdef1234-dirty"
#   包含: 版本号 + git 信息 + 脏标记
# UBOOTVERSION: 基础版本字符串（不含 git 信息）
#   内容示例: "2025.04-rc3"
UBOOTRELEASE = $(shell cat include/config/uboot.release 2> /dev/null)
UBOOTVERSION = $(VERSION)$(if $(PATCHLEVEL),.$(PATCHLEVEL)$(if $(SUBLEVEL),.$(SUBLEVEL)))$(EXTRAVERSION)

# 将所有关键变量导出到子 make 进程和 shell 命令
# 按功能分组：
#   版本信息 → 子 Makefile 和链接脚本中使用
#   架构信息 → 决定编译哪些目录和文件
#   工具链   → 所有编译命令都需要
#   编译标志 → 统一的编译选项
export VERSION PATCHLEVEL SUBLEVEL UBOOTRELEASE UBOOTVERSION
export ARCH CPU BOARD VENDOR SOC CPUDIR BOARDDIR
export CONFIG_SHELL HOSTCC KBUILD_HOSTCFLAGS CROSS_COMPILE AS LD CC
export CPP AR NM LDR STRIP OBJCOPY OBJDUMP KBUILD_HOSTLDFLAGS KBUILD_HOSTLDLIBS
export LEX YACC
export KBUILD_CPPFLAGS KBUILD_CFLAGS KBUILD_AFLAGS
export UBOOTINCLUDE KBUILD_LDFLAGS

export CC_VERSION_TEXT := $(shell $(CC) --version | head -n 1)

export RCS_FIND_IGNORE := \( -name SCCS -o -name BitKeeper -o -name .svn -o    \
			  -name CVS -o -name .pc -o -name .hg -o -name .git \) \
			  -prune -o

# $(build) 在 Kbuild.include 中定义为:
#   build := -f $(srctree)/scripts/Makefile.build obj
# 因此 $(Q)$(MAKE) $(build)=scripts/basic 展开为:
#   @make -f scripts/Makefile.build obj=scripts/basic
PHONY += scripts_basic
scripts_basic:
	$(Q)$(MAKE) $(build)=scripts/basic

# 防止 scripts/basic/ 下的文件触发隐式规则
# 空规则确保 make 不会尝试"构建"这些文件
scripts/basic/%: scripts_basic ;

# outputmakefile: 在外部构建目录中生成辅助 Makefile
# 这样用户可以在输出目录中直接运行 make，无需指定 -f
PHONY += outputmakefile
outputmakefile:
ifneq ($(KBUILD_SRC),)
	$(Q)ln -fsn $(srctree) source
	$(Q)$(CONFIG_SHELL) $(srctree)/scripts/mkmakefile $(srctree)
endif

# 版本和时间戳头文件路径
# 这些文件在 prepare 阶段生成，供 C 代码引用
version_h := include/generated/version_autogenerated.h
timestamp_h := include/generated/timestamp_autogenerated.h

# 不需要 .config 的目标列表
# clean/mrproper/distclean: 清理目标
# ubootversion: 只打印版本号
no-dot-config-targets := clean mrproper distclean \
			 ubootversion

# 三个控制标志的初始值
config-targets := 0
mixed-targets  := 0
dot-config     := 1

# 检查当前目标是否全部为"不需要 .config"的目标
# 逻辑：
#   如果 MAKECMDGOALS 中有 no-dot-config-targets 的成员，则继续检查
#     如果过滤掉这些目标后为空，全部是不需要 .config 的，则dot-config := 0
#     否则，还有其他目标需要 .config，保持 dot-config := 1
ifneq ($(filter $(no-dot-config-targets), $(MAKECMDGOALS)),)
	ifeq ($(filter-out $(no-dot-config-targets), $(MAKECMDGOALS)),)
		dot-config := 0
	endif
endif

# 检查是否包含配置目标
# "config" 匹配纯 "config" 目标
# "%config" 匹配 menuconfig, defconfig, oldconfig 等
ifneq ($(filter config %config,$(MAKECMDGOALS)),)
        config-targets := 1
        # 如果目标多于 1 个（即除了 *config 还有其他），则为混合目标
        ifneq ($(words $(MAKECMDGOALS)),1)
                mixed-targets := 1
        endif
endif

# 混合目标处理
ifeq ($(mixed-targets),1)
# 当用户执行类似 "make defconfig all" 时：
# 不能同时处理配置和构建，因为 .config 必须先生成
# 解决方案：逐个目标递归调用 make
#   1. make -f Makefile defconfig
#   2. make -f Makefile all

PHONY += $(MAKECMDGOALS) __build_one_by_one

# 除 __build_one_by_one 外的所有目标都依赖它
# @: 空命令——这些目标本身不做任何事，实际工作在 __build_one_by_one 中
$(filter-out __build_one_by_one, $(MAKECMDGOALS)): __build_one_by_one
	@:

# 核心逻辑：用 set -e 确保任一步骤失败立即停止
# 然后 for 循环逐个调用 make
__build_one_by_one:
	$(Q)set -e; \
	for i in $(MAKECMDGOALS); do \
		$(MAKE) -f $(srctree)/Makefile $$i; \
	done

else # mixed-targets=0

# 配置目标处理
ifeq ($(config-targets),1)

# KBUILD_DEFCONFIG: 默认的 defconfig 文件
KBUILD_DEFCONFIG := 
export KBUILD_DEFCONFIG KBUILD_KCONFIG

# config 和 %config 规则：
# 依赖：
#   scripts_basic — 确保 fixdep 等基础工具已编译
#   outputmakefile — 确保外部构建的 Makefile 已生成
#   FORCE — 强制每次都执行（即使目标文件存在）
#
# 动作：
#   进入 scripts/kconfig 目录执行对应目标
#   scripts/kconfig/Makefile 会编译并运行：
#     conf  — 命令行配置工具（处理 defconfig, oldconfig, syncconfig）
#     mconf — ncurses 图形界面（处理 menuconfig）
#     nconf — 更现代的 ncurses 界面（处理 nconfig）
#
# 例如 make menuconfig 展开为：
#   @make -f scripts/Makefile.build obj=scripts/kconfig menuconfig
config: scripts_basic outputmakefile FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

%config: scripts_basic outputmakefile FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

else # config-targets=0

# 构建目标处理
# 处理所有非配置目标：u-boot.bin、all、clean 等
# include/config.mk 由 Kconfig 的 syncconfig 生成
# -include: 文件不存在时不报错（首次构建时该文件尚未生成）
-include include/config.mk

# scripts 目标：编译辅助脚本工具
# 仅在 auto.conf 存在后才编译，确保配置已完成
PHONY += scripts
scripts: scripts_basic include/config/auto.conf
	$(Q)$(MAKE) $(build)=$(@)

# .config 与 auto.conf 同步机制
ifeq ($(dot-config),1)

# 读取 auto.conf
# 这些变量在后续 Makefile 逻辑中作为条件判断使用
-include include/config/auto.conf

# auto.conf.cmd 记录了生成 auto.conf 时的 Kconfig 依赖
# 这样当任何 Kconfig 文件改变时，auto.conf 会自动重新生成
-include include/config/auto.conf.cmd

# 防止 make 尝试用隐式规则重建这些文件
$(KCONFIG_CONFIG) include/config/auto.conf.cmd: ;

include/config/%.conf: $(KCONFIG_CONFIG) include/config/auto.conf.cmd
	$(Q)$(MAKE) -f $(srctree)/Makefile syncconfig
	@# 如果 Makefile.autoconf 执行失败，删除 auto.conf
	@# 这样下次 make 时会重新触发 syncconfig
	$(Q)$(MAKE) -f $(srctree)/scripts/Makefile.autoconf || \
		{ rm -f include/config/auto.conf; false; }
	@# 更新 auto.conf 的时间戳
	@# 防止 auto.conf 比 autoconf.h 旧导致循环触发
	$(Q)touch include/config/auto.conf

# u-boot.cfg: 生成完整的预处理配置文件
# 用于调试——可以看到所有 CONFIG_xxx 的最终值
u-boot.cfg:
	$(Q)$(MAKE) -f $(srctree)/scripts/Makefile.autoconf $(@)

# 旧式兼容性配置文件
# autoconf.mk: 类似 auto.conf 但格式略有不同
# autoconf.mk.dep: autoconf.mk 的依赖文件
-include include/autoconf.mk
-include include/autoconf.mk.dep

# 三重条件守卫确保只在配置完全就绪时才加载架构 Makefile：
#   1) .config 文件存在
#   2) auto.conf 文件存在
#   3) auto.conf 不比 .config 旧（用户没有在生成后修改 .config）
#
# config.mk: 传统的板级配置（ARCH, CPU, BOARD 等变量）
# arch/$(ARCH)/Makefile: 架构级编译规则
#   定义 head-y（启动代码）, PLATFORM_CPPFLAGS 等关键变量
ifneq ($(wildcard $(KCONFIG_CONFIG)),)
ifneq ($(wildcard include/config/auto.conf),)
autoconf_is_old := $(shell find . -path ./$(KCONFIG_CONFIG) -newer \
						include/config/auto.conf)
ifeq ($(autoconf_is_old),)
include config.mk
include arch/$(ARCH)/Makefile
endif
endif
endif

ifndef LDSCRIPT
	ifdef CONFIG_SYS_LDSCRIPT
		LDSCRIPT := $(srctree)/$(CONFIG_SYS_LDSCRIPT:"%"=%)
	endif
endif

ifndef LDSCRIPT
	ifeq ($(wildcard $(LDSCRIPT)),)
		LDSCRIPT := $(srctree)/board/$(BOARDDIR)/u-boot.lds
	endif
	ifeq ($(wildcard $(LDSCRIPT)),)
		LDSCRIPT := $(srctree)/$(CPUDIR)/u-boot.lds
	endif
	ifeq ($(wildcard $(LDSCRIPT)),)
		LDSCRIPT := $(srctree)/arch/$(ARCH)/cpu/u-boot.lds
	endif
endif

else #dot-config=0

# dot-config=0 时（clean 等目标），auto.conf 不需要存在
# 提供空规则防止 make 报错
include/config/auto.conf: ;

endif # $(dot-config)

ifdef CONFIG_CC_OPTIMIZE_FOR_SIZE
# -Os: 优化代码体积（bootloader 通常受 ROM/Flash 大小限制）
# 这是 U-Boot 最常用的优化级别
KBUILD_CFLAGS	+= -Os
endif

ifdef CONFIG_CC_OPTIMIZE_FOR_SPEED
# -O2: 优化执行速度（某些性能敏感场景）
KBUILD_CFLAGS	+= -O2
endif

ifdef CONFIG_CC_OPTIMIZE_FOR_DEBUG
# -Og: 优化调试体验
# 在不严重影响调试的前提下进行最小优化
# 比 -O0 生成更好的代码，同时保持大部分变量可观察
KBUILD_CFLAGS	+= -Og
# GCC 在 -Og 下可能产生误报的 "maybe-uninitialized" 警告
# 参见 GCC Bug #78394
KBUILD_CFLAGS	+= -Wno-maybe-uninitialized
endif

# LTO (Link-Time Optimization) 链接时优化标志
# 允许编译器在链接阶段跨编译单元进行优化
# 此处初始化为空，由架构 Makefile 按需设置
LTO_CFLAGS :=
LTO_FINAL_LDFLAGS :=
export LTO_CFLAGS LTO_FINAL_LDFLAGS

UBOOTINCLUDE    := \
	-Iinclude \
	$(if $(KBUILD_SRC), -I$(srctree)/include) \
	-I$(srctree)/arch/$(ARCH)/include	\
	-include $(srctree)/include/linux/kconfig.h

# -nostdinc: 不搜索标准系统头文件目录
#   U-Boot 是独立环境，不能使用宿主机的 stdio.h 等
# -isystem: 但仍需要编译器内建头文件（如 stdarg.h, stddef.h, stdint.h），这些是编译器提供的，与操作系统无关
#   $(CC) -print-file-name=include 输出编译器内建头文件路径
#   例如: /usr/lib/gcc/arm-linux-gnueabihf/11/include
NOSTDINC_FLAGS += -nostdinc -isystem $(shell $(CC) -print-file-name=include)

# 组合完整的预处理和编译标志
cpp_flags := $(KBUILD_CPPFLAGS) $(PLATFORM_CPPFLAGS) $(UBOOTINCLUDE) \
							$(NOSTDINC_FLAGS)
c_flags := $(KBUILD_CFLAGS) $(cpp_flags)

libs-y += common/
libs-y += drivers/

# sort 去重并按字母排序
# 排序虽然改变了链接顺序，但 U-Boot 的启动入口由链接脚本的 ENTRY() 指令决定，而非链接顺序
libs-y := $(sort $(libs-y))

# u-boot-dirs: 去掉尾部 '/' 得到纯目录名列表
# filter %/: 过滤出以 '/' 结尾的项（即需要递归的目录）
# patsubst %/,%: 去掉尾部的 '/'
# 例如: "common/ lib/ drivers/" → "common lib drivers"
u-boot-dirs	:= $(patsubst %/,%,$(filter %/, $(libs-y))) tools

# u-boot-alldirs: 包含所有目录（含 libs- 即被禁用的目录）
# 用于 clean 等操作——即使被禁用的目录也需要清理
u-boot-alldirs	:= $(sort $(u-boot-dirs) $(patsubst %/,%,$(filter %/, $(libs-))))

# 将目录名转换为 built-in.o 路径
# 例如: "common/ lib/" → "common/built-in.o lib/built-in.o"
# built-in.o 是每个目录的编译产物集合（由 Makefile.build 生成）
# 它是该目录下所有 obj-y 目标的归档文件
libs-y		:= $(patsubst %/, %/built-in.o, $(libs-y))

# u-boot-init: 启动代码（必须最先链接）
# head-y 由架构 Makefile 定义，通常指向:arch/arm/cpu/armv7/start.o （ARM 示例）
# 这是 U-Boot 的入口点，包含复位向量和初始化代码
u-boot-init := $(head-y)

# u-boot-main: 其余所有编译产物
u-boot-main := $(libs-y)

# 链接脚本 (.lds) 通常需要预处理以支持条件编译
# LDPPFLAGS 是传给 CPP 的标志：
#   -DCPUDIR: 传递 CPU 目录路径
#   -DLD_MAJOR/-DLD_MINOR: 链接器版本号，用于兼容性处理
LDPPFLAGS += \
	-DCPUDIR=$(CPUDIR) \
	$(shell $(LD) --version | \
	  sed -ne 's/GNU ld version \([0-9][0-9]*\)\.\([0-9][0-9]*\).*/-DLD_MAJOR=\1 -DLD_MINOR=\2/p')

# INPUTS-y: 最终要生成的二进制文件列表
INPUTS-y += u-boot.bin

# 链接标志
# LDFLAGS_FINAL: 由架构 Makefile 定义的最终链接选项
# -Ttext: 指定 .text 段的加载地址
# CONFIG_TEXT_BASE 在 Kconfig 中配置，例如 0x87800000
LDFLAGS_u-boot += $(LDFLAGS_FINAL)
LDFLAGS_u-boot += -Ttext $(CONFIG_TEXT_BASE)

# objcopy 标志：将 ELF 转换为纯二进制（raw binary）
# -O binary: 输出格式为原始二进制
# 结果文件可以直接烧写到 Flash 或通过 JTAG 加载
OBJCOPYFLAGS_u-boot.bin := -O binary

# 命令定义
# Kbuild 的命令定义约定：
#   quiet_cmd_xxx: V=0 时显示的简短信息
#   cmd_xxx:       实际执行的命令
# $(call if_changed,xxx) 会：
#   1) 检查命令是否与上次相同（通过 .cmd 文件）
#   2) 检查依赖是否有变化
#   3) 如果需要重新执行，根据 $(quiet) 选择输出格式
quiet_cmd_objcopy = OBJCOPY $@
cmd_objcopy = $(OBJCOPY) $(OBJCOPYFLAGS) \
	$(OBJCOPYFLAGS_$(@F)) $< $@

quiet_cmd_objcopy_uboot = OBJCOPY $@
cmd_objcopy_uboot = $(cmd_objcopy)

# cfg 目标：生成 u-boot.cfg 配置摘要文件
cfg: u-boot.cfg

# binman 集成
# binman 是 U-Boot 的二进制打包工具
# 它负责将 u-boot.bin、SPL、ATF、设备树等组合成最终的烧写镜像
# .binman_stamp 是时间戳文件，确保每次构建都运行 binman
.binman_stamp: $(INPUTS-y) FORCE
ifeq ($(CONFIG_BINMAN),y)
	$(call if_changed,binman)
endif
	@touch $@

# all 是默认构建目标，依赖 .binman_stamp
all: .binman_stamp

# u-boot.bin 生成规则
# u-boot (ELF) → u-boot.bin (raw binary)
# FORCE 确保每次 make 都检查是否需要重新生成
u-boot.bin: u-boot FORCE
	$(call if_changed,objcopy_uboot)

# u-boot (ELF) 链接规则
# 这是整个构建系统最核心的规则——将所有目标文件链接为 ELF
#
# cmd_u-boot__ 的执行步骤：
#   1) touch $(u-boot-main): 确保 built-in.o 文件存在（防止空目录报错）
#   2) $(LD) 链接：
#      $(KBUILD_LDFLAGS): 全局链接选项
#      $(LDFLAGS_u-boot): u-boot 特定选项（-Ttext 等）
#      -o $@:             输出文件 u-boot
#      -T u-boot.lds:     使用链接脚本
#      $(u-boot-init):    启动代码（head-y，如 start.o）
#      --whole-archive：这是一个链接器选项，告诉链接器将后面跟的归档文件（.a）中的所有目标文件都链接进来，
#      即使其中没有符号被引用。默认情况下，链接器只会从归档文件中提取那些解决了未定义符号的模块。
#      $(u-boot-main):    主体代码（各目录的 built-in.o）被包裹在 --whole-archive 和 --no-whole-archive 之间，
#      以确保所有必要的符号（如驱动入口、初始化函数）都被强制包含，避免因符号未引用而被丢弃。
#      --no-whole-archive：关闭 --whole-archive 效果，后续的库恢复正常链接行为（只提取必要符号）。
#      -Map u-boot.map:   生成内存映射文件，用于调试
quiet_cmd_u-boot__ ?= LD      $@
      cmd_u-boot__ ?=								\
		touch $(u-boot-main) ;						\
		$(LD) $(KBUILD_LDFLAGS) $(LDFLAGS_u-boot) -o $@			\
		-T u-boot.lds $(u-boot-init)					\
		--whole-archive							\
			$(u-boot-main)						\
		--no-whole-archive						\
		-Map u-boot.map

# u-boot 目标规则
# 依赖：
#   $(u-boot-init): 启动代码 .o 文件
#   $(u-boot-main): 各目录的 built-in.o
#   u-boot.lds:     链接脚本（已预处理）
#   FORCE:          强制检查
#
# +$(call if_changed,...): 前缀 '+' 的作用：
#   即使 make 以 -n (dry-run) 或 -t (touch) 模式运行，
#   也强制执行此命令。这是因为链接步骤对于验证构建至关重要
u-boot:	$(u-boot-init) $(u-boot-main) u-boot.lds FORCE
	+$(call if_changed,u-boot__)

# built-in.o 文件本身不需要显式规则
# 它们在递归进入子目录时由 Makefile.build 自动生成
# 空规则防止隐式规则干扰
# sort 去重，避免同一目标被执行多次
$(sort $(u-boot-init) $(u-boot-main)): $(u-boot-dirs) ;

# 核心递归规则：
# 对 u-boot-dirs 中的每个目录，执行：
#   make -f scripts/Makefile.build obj=<dir>
#
# Makefile.build 是 Kbuild 的核心递归脚本，它会：
#   1) 进入目标目录
#   2) 读取该目录的 Makefile（或 Kbuild 文件）
#   3) 获取 obj-y += xxx.o 列表
#   4) 编译所有 .c/.S → .o
#   5) 将 obj-y 归档为 built-in.o
#   6) 如果 obj-y 中有目录（如 obj-y += subdir/），递归处理
#
# 依赖 prepare 和 scripts：确保头文件、配置、工具都已就绪
#
# Locale 设置注释说明：
#   这里不再修改 locale，因为全局已设置为 C locale
#   错误信息仍使用用户的原始语言（未 export LC_MESSAGES）
PHONY += $(u-boot-dirs)
$(u-boot-dirs): prepare scripts
	$(Q)$(MAKE) $(build)=$@

# 版本信息生成
# setlocalversion 脚本来自 Linux 内核
# 它生成类似 "-00001-gabcdef1234-dirty" 的版本后缀
# KERNELVERSION 变量是脚本所需的环境变量（沿用 Linux 的命名）
define filechk_uboot.release
	KERNELVERSION=$(UBOOTVERSION) $(CONFIG_SHELL) $(srctree)/scripts/setlocalversion $(srctree)
endef

# filechk 宏（定义在 Kbuild.include 中）的工作方式：
#   1) 执行 filechk_xxx 生成新内容到临时文件
#   2) 与现有文件比较
#   3) 仅在内容改变时才替换文件
#   4) 这避免了不必要的重编译（文件时间戳不变，依赖不触发）
include/config/uboot.release: include/config/auto.conf FORCE
	$(call filechk,uboot.release)

# 构建准备阶段
# prepare 是一个多阶段的准备链，从 prepare3 到 prepare0 逐级执行：
#   prepare3: 外部构建环境检查
#   prepare2: 生成输出目录的 Makefile
#   prepare1: 生成版本头文件 + 检查链接脚本
#   archprepare: 架构特定准备 + 编译工具
#   prepare0: 处理根目录的 Kbuild 文件
#   prepare: 最终就绪（其他规则依赖此目标）

PHONY += prepare archprepare prepare0 prepare1 prepare2 prepare3

# prepare3: 外部构建时的完整性检查
# 确保源码目录是"干净"的——不包含之前构建的残留物
# 如果源码目录中有 .config 或 include/config/，说明有人在源码目录中执行过构建，这可能导致冲突
prepare3: include/config/uboot.release
ifneq ($(KBUILD_SRC),)
	@$(kecho) '  Using $(srctree) as source for U-Boot'
	$(Q)if [ -f $(srctree)/.config -o -d $(srctree)/include/config ]; then \
		echo >&2 "  $(srctree) is not clean, please run 'make mrproper'"; \
		echo >&2 "  in the '$(srctree)' directory.";\
		/bin/false; \
	fi;
endif

# prepare2: 确保输出目录有可用的 Makefile
# cfg 确保include/config.h等文件生成，尤其是make clean之后
prepare2: prepare3 outputmakefile cfg

# prepare1: 生成核心头文件 + 配置验证
# 依赖：
#   version_h:  include/generated/version_autogenerated.h
#   timestamp_h: include/generated/timestamp_autogenerated.h
#   auto.conf:  确保配置已同步
# 检查链接脚本是否存在（LDSCRIPT 为空或指向不存在的文件时报错）
prepare1: prepare2 $(version_h) $(timestamp_h) \
				include/config/auto.conf
ifeq ($(wildcard $(LDSCRIPT)),)
	@echo >&2 "  Could not find linker script."
	@/bin/false
endif

# archprepare: 编译架构相关的准备工具
# tools 目标会编译如 imxdownload 等镜像打包工具
archprepare: prepare1 scripts_basic

# prepare0: 处理根目录的 Kbuild 文件
# $(Q)$(MAKE) $(build)=. 会读取根目录的 Kbuild 文件
# 用于生成根目录级的头文件或配置
prepare0: archprepare FORCE
	$(Q)$(MAKE) $(build)=.

# 最终的 prepare 目标——所有子目录编译前的"总集结点"
prepare: prepare0

# filechk_version.h: 生成版本信息头文件
define filechk_version.h
	(echo \#define PLAIN_VERSION \"$(UBOOTRELEASE)\"; \
	echo \#define U_BOOT_VERSION \"U-Boot \" PLAIN_VERSION; \
	echo \#define U_BOOT_VERSION_NUM $(VERSION); \
	echo \#define U_BOOT_VERSION_NUM_PATCH $$(echo $(PATCHLEVEL) | \
		sed -e "s/^0*//"); \
	echo \#define HOST_ARCH $(HOST_ARCH); \
	echo \#define CC_VERSION_STRING \"$$(LC_ALL=C $(CC) --version | head -n 1)\"; \
	echo \#define LD_VERSION_STRING \"$$(LC_ALL=C $(LD) --version | head -n 1)\"; )
endef

# filechk_timestamp.h: 生成构建时间戳头文件
# 支持 SOURCE_DATE_EPOCH 环境变量实现可复现构建（Reproducible Build）：
#   如果设置了 SOURCE_DATE_EPOCH（Unix 时间戳），使用该固定时间
#   否则使用当前时间
define filechk_timestamp.h
	(if test -n "$${SOURCE_DATE_EPOCH}"; then \
		SOURCE_DATE="@$${SOURCE_DATE_EPOCH}"; \
		DATE=""; \
		for date in gdate date.gnu date; do \
			$${date} -u -d "$${SOURCE_DATE}" >/dev/null 2>&1 && DATE="$${date}"; \
		done; \
		if test -n "$${DATE}"; then \
			LC_ALL=C $${DATE} -u -d "$${SOURCE_DATE}" +'#define U_BOOT_DATE "%b %d %C%y"'; \
			LC_ALL=C $${DATE} -u -d "$${SOURCE_DATE}" +'#define U_BOOT_TIME "%T"'; \
			LC_ALL=C $${DATE} -u -d "$${SOURCE_DATE}" +'#define U_BOOT_TZ "%z"'; \
			LC_ALL=C $${DATE} -u -d "$${SOURCE_DATE}" +'#define U_BOOT_EPOCH %s'; \
		else \
			return 42; \
		fi; \
	else \
		LC_ALL=C date +'#define U_BOOT_DATE "%b %d %C%y"'; \
		LC_ALL=C date +'#define U_BOOT_TIME "%T"'; \
		LC_ALL=C date +'#define U_BOOT_TZ "%z"'; \
		LC_ALL=C date +'#define U_BOOT_EPOCH %s'; \
	fi)
endef

# version_h 和 timestamp_h 的生成规则
# filechk 宏确保只在内容真正改变时才更新文件
# 这避免了因时间戳变化导致的不必要重编译链
$(version_h): include/config/uboot.release FORCE
	$(call filechk,version.h)

# timestamp_h 依赖顶层 Makefile（因为 VERSION 等变量在此定义）
$(timestamp_h): $(srctree)/Makefile FORCE
	$(call filechk,timestamp.h)

# 工具编译
# tools 目标：编译构建过程中使用的宿主机工具
# tools_imxdownload: NXP i.MX 系列处理器的镜像下载工具
# 在实际 U-Boot 中还包括 mkimage、mkenvimage 等
PHONY += tools
tools: tools_imxdownload

# imxdownload 工具编译
PHONY += tools_imxdownload
tools_imxdownload: scripts_basic
	$(Q)$(MAKE) $(build)=tools

# 链接脚本预处理
# U-Boot 的链接脚本（.lds）需要 C 预处理器处理
# 原因：链接脚本中使用了 #include、#ifdef、CONFIG_xxx 等
# 标志说明：
#   -Wp,-MD,$(depfile): 生成依赖文件（用于增量编译）
#   $(cpp_flags):        C 预处理标志（包含路径、宏定义等）
#   $(LDPPFLAGS):        链接脚本专用标志（CPUDIR、LD 版本等）
#   -D__ASSEMBLY__:      告知头文件这不是 C 编译
#   -x assembler-with-cpp: 告知 GCC 输入是需要预处理的汇编/脚本
#   -std=c99:            使用 C99 预处理器标准
#   -P:                  不生成行号标记（#line），链接器不识别它们
quiet_cmd_cpp_lds = LDS     $@
cmd_cpp_lds = $(CPP) -Wp,-MD,$(depfile) $(cpp_flags) $(LDPPFLAGS) \
		-D__ASSEMBLY__ -x assembler-with-cpp -std=c99 -P -o $@ $<

# u-boot.lds 生成规则
# if_changed_dep: 同 if_changed，但额外处理依赖文件（.d to .cmd 转换）
u-boot.lds: $(LDSCRIPT) prepare FORCE
	$(call if_changed_dep,cpp_lds)

# Directories & files removed with 'make clean'
CLEAN_DIRS  += 

CLEAN_FILES += include/autoconf.mk* include/config.h u-boot* System.map \
	       defconfig

# Directories & files removed with 'make mrproper'
MRPROPER_DIRS  += include/config include/generated include/asm

# MRPROPER_FILES: 需要精确删除的文件（包含配置文件）
MRPROPER_FILES += .config .config.old include/autoconf.mk* include/config.h

# GNU Make 的"目标特定变量"特性：
#   clean: rm-dirs := $(CLEAN_DIRS)
# 等价于：
#   在执行 clean 目标时，临时设置 rm-dirs = $(CLEAN_DIRS)
#   执行完毕后，rm-dirs 恢复原值（如果有）rm-dirs 只在 clean 目标及其依赖中有效
clean: rm-dirs  := $(CLEAN_DIRS)
clean: rm-files := $(CLEAN_FILES)

# 收集需要递归清理的子目录
# u-boot-alldirs包含所有 libs-y 和 libs- 的目录
# 逻辑：
#   1) 遍历 u-boot-alldirs（所有源码目录，包括被禁用的）
#   2) 检查每个目录是否存在 Makefile（$(wildcard $(srctree)/$f/Makefile)）
#   3) 只有存在 Makefile 的目录才需要递归清理
clean-dirs	:= $(foreach f,$(u-boot-alldirs),$(if $(wildcard $(srctree)/$f/Makefile),$f))

# 为每个目录添加 _clean_ 前缀，构造伪目标名称
clean-dirs      := $(addprefix _clean_, $(clean-dirs))

PHONY += $(clean-dirs) clean archclean
$(clean-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _clean_%,%,$@)

clean: $(clean-dirs)
	$(call cmd,rmdirs)
	$(call cmd,rmfiles)
	@find . $(RCS_FIND_IGNORE) \
		\( -name '*.[oas]' -o -name '.*.cmd' \
		-o -name '.*.d' -o -name '.*.tmp' \
		-o -name '*.lex.c' -o -name '*.tab.[ch]' \
		-o -name 'generated_defconfig' \
		-o -name '*.efi' -o -name '*.gcno' -o -name '*.so' \) \
		-type f -print | xargs rm -f

# mrproper - Delete all generated files, including .config
#
mrproper: rm-dirs  := $(wildcard $(MRPROPER_DIRS))
mrproper: rm-files := $(wildcard $(MRPROPER_FILES))
mrproper-dirs      := $(addprefix _mrproper_,scripts)

PHONY += $(mrproper-dirs) mrproper archmrproper
$(mrproper-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _mrproper_%,%,$@)

mrproper: clean $(mrproper-dirs)
	$(call cmd,rmdirs)
	$(call cmd,rmfiles)
	@rm -f arch/*/include/asm/arch

# distclean
#
PHONY += distclean

distclean: mrproper
	@find $(srctree) $(RCS_FIND_IGNORE) \
		\( -name '*.orig' -o -name '*.rej' -o -name '*~' \
		-o -name '*.bak' -o -name '#*#' -o -name '.*.orig' \
		-o -name '.*.rej' -o -name '*%' -o -name 'core' \
		-o -name '*.pyc' \) \
		-type f -print | xargs rm -f

quiet_cmd_rmdirs = $(if $(wildcard $(rm-dirs)),CLEAN   $(wildcard $(rm-dirs)))
      cmd_rmdirs = rm -rf $(rm-dirs)

quiet_cmd_rmfiles = $(if $(wildcard $(rm-files)),CLEAN   $(wildcard $(rm-files)))
      cmd_rmfiles = rm -f $(rm-files)

# .cmd 文件是 Kbuild 增量编译系统的核心
# 每次编译命令执行后，if_changed 宏会生成对应的 .cmd 文件
# 下次 make 时：
#   1) include .main.o.cmd，加载上次的命令和依赖
#   2) if_changed 比较当前命令与 cmd_main.o
#   3) 如果命令改变（如 CFLAGS 变了），重新编译
#   4) 如果依赖文件改变，重新编译
#   5) 都没变，跳过编译

# 找到所有 .cmd 文件
cmd_files := $(wildcard .*.cmd)

ifneq ($(cmd_files),)
  # 空规则防止 make 尝试重建 .cmd 文件
  $(cmd_files): ;
  # 包含所有 .cmd 文件，加载历史命令和依赖信息
  include $(cmd_files)
endif

endif #ifeq ($(config-targets),1)
endif #ifeq ($(mixed-targets),1)
endif	# skip-makefile

# FORCE 目标：Kbuild 的"万能依赖"
# 它永远被视为"已更新"（因为没有规则来创建它，也不是文件）
# 任何依赖 FORCE 的目标都会被重新评估（但不一定重新执行）
#
# 与 .PHONY 的区别：
#   .PHONY 目标总是执行其命令
#   依赖 FORCE 的目标只在命令或依赖改变时执行（通过 if_changed 检查）
#
# 这是一个关键的优化：
#   例如 u-boot.bin 依赖 FORCE，但如果 u-boot 没有改变，if_changed 会跳过 objcopy，避免不必要的工作
PHONY += FORCE
FORCE:

# 统一声明所有伪目标
# 将 PHONY 变量中收集的所有目标名声明为 .PHONY
# 这比在每个规则处单独声明更清晰、更不易遗漏
.PHONY: $(PHONY)
