# SPDX-License-Identifier: GPL-2.0
# ===========================================================================
# Kernel configuration targets
# These targets are used from top-level makefile

PHONY += menuconfig config localyesconfig build_menuconfig

# Added for U-Boot
#  Linux has defconfig files in arch/$(SRCARCH)/configs/,
#  on the other hand, U-Boot does in configs/.
#  Set SRCARCH to .. fake this Makefile.
SRCARCH := ..

ifdef KBUILD_KCONFIG
Kconfig := $(KBUILD_KCONFIG)
else
Kconfig := Kconfig
endif

ifeq ($(quiet),silent_)
silent := -s
endif

# We need this, in case the user has it in its environment
unexport CONFIG_

menuconfig: $(obj)/mconf
	$< $(silent) $(Kconfig)

config: $(obj)/conf
	$< $(silent) --oldaskconfig $(Kconfig)

build_menuconfig: $(obj)/mconf

simple-targets := oldconfig syncconfig
PHONY += $(simple-targets)
$(simple-targets): $(obj)/conf
	$< $(silent) --$@ $(Kconfig)

PHONY += savedefconfig

savedefconfig: $(obj)/conf
	$< $(silent) --$@=defconfig $(Kconfig)

%_defconfig: $(obj)/conf
	$(Q)$(CPP) -nostdinc -P -I $(srctree) -undef -x assembler-with-cpp $(srctree)/arch/$(SRCARCH)/configs/$@ -o generated_defconfig
	$(Q)$< $(silent) --defconfig=generated_defconfig $(Kconfig)

# ===========================================================================
# Shared Makefile for the various kconfig executables:
# conf:	  Used for defconfig, oldconfig and related targets
# object files used by all kconfig flavours

conf-objs	:= conf.o  zconf.tab.o

hostprogs-y := conf

targets		+= zconf.lex.c

# generated files seem to need this to find local include files
HOSTCFLAGS_zconf.lex.o	:= -I$(src)
HOSTCFLAGS_zconf.tab.o	:= -I$(src)

# mconf: Used for the menuconfig target based on lxdialog
hostprogs-y	+= mconf
lxdialog	:= checklist.o inputbox.o menubox.o textbox.o util.o yesno.o
mconf-objs	:= mconf.o zconf.tab.o $(addprefix lxdialog/, $(lxdialog))

HOSTLDLIBS_mconf = $(shell . $(obj)/.mconf-cfg && echo $$libs)
$(foreach f, mconf.o $(lxdialog), \
  $(eval HOSTCFLAGS_$f = $$(shell . $(obj)/.mconf-cfg && echo $$$$cflags)))

$(obj)/mconf.o: $(obj)/.mconf-cfg
$(addprefix $(obj)/lxdialog/, $(lxdialog)): $(obj)/.mconf-cfg

$(obj)/zconf.tab.o: $(obj)/zconf.lex.c

# check if necessary packages are available, and configure build flags
define filechk_conf_cfg
	$(CONFIG_SHELL) $<
endef

$(obj)/.%conf-cfg: $(src)/%conf-cfg.sh FORCE
	$(call filechk,conf_cfg)

clean-files += .*conf-cfg
