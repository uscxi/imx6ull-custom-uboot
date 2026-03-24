# SPDX-License-Identifier: GPL-2.0+
#
# (C) Copyright 2000-2002
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.

FIXED_REG := -ffixed-r9

ifneq ($(LTO_ENABLE),y)
PLATFORM_RELFLAGS += -ffunction-sections -fdata-sections
endif

PLATFORM_RELFLAGS += -fno-common $(FIXED_REG)
PLATFORM_RELFLAGS += $(call cc-option, -msoft-float) \
		     $(call cc-option,-mgeneral-regs-only) \
      $(call cc-option,-mshort-load-bytes,$(call cc-option,-malignment-traps,))

# LLVM support
LLVM_RELFLAGS		:= $(call cc-option,-mllvm,) \
			$(call cc-option,-mno-movt,)
PLATFORM_RELFLAGS	+= $(LLVM_RELFLAGS)

PLATFORM_CPPFLAGS += -D__ARM__

PLATFORM_ELFFLAGS += -B arm -O elf32-littlearm

# Choose between ARM/Thumb instruction sets
ifeq ($(CONFIG_SYS_THUMB_BUILD),y)
AFLAGS_IMPLICIT_IT	:= $(call as-option,-Wa$(comma)-mimplicit-it=always)
PF_CPPFLAGS_ARM		:= $(AFLAGS_IMPLICIT_IT) \
			$(call cc-option, -mthumb -mthumb-interwork,\
			$(call cc-option,-marm,)\
			$(call cc-option,-mno-thumb-interwork,)\
		)
else
PF_CPPFLAGS_ARM := $(call cc-option,-marm,) \
		$(call cc-option,-mno-thumb-interwork,)
endif

# Only test once
ifeq ($(CONFIG_SYS_THUMB_BUILD),y)
archprepare: checkthumb checkgcc6

checkthumb:
	@if test "$(call cc-name)" = "gcc" -a \
			"$(call cc-version)" -lt "0404"; then \
		echo -n '*** Your GCC does not produce working '; \
		echo 'binaries in THUMB mode.'; \
		echo '*** Your board is configured for THUMB mode.'; \
		false; \
	fi
else
archprepare: checkgcc6
endif

checkgcc6:
	@if test "$(call cc-name)" = "gcc" -a \
			"$(call cc-version)" -lt "0600"; then \
		echo '*** Your GCC is older than 6.0 and is not supported'; \
		false; \
	fi

# Try if EABI is supported, else fall back to old API,
# i. e. for example:
# - with ELDK 4.2 (EABI supported), use:
#	-mabi=aapcs-linux
# - with ELDK 4.1 (gcc 4.x, no EABI), use:
#	-mabi=apcs-gnu
# - with ELDK 3.1 (gcc 3.x), use:
#	-mapcs-32
PF_CPPFLAGS_ABI := $(call cc-option,\
			-mabi=aapcs-linux,\
			$(call cc-option,\
				-mapcs-32,\
				$(call cc-option,\
					-mabi=apcs-gnu,\
				)\
			)\
		)
PLATFORM_CPPFLAGS += $(PF_CPPFLAGS_ARM) $(PF_CPPFLAGS_ABI)

# needed for relocation
LDFLAGS_u-boot += -pie

ifneq ($(CONFIG_XPL_BUILD),y)
# Check that only R_ARM_RELATIVE relocations are generated.
INPUTS-y += checkarmreloc
# The movt / movw can hardcode 16 bit parts of the addresses in the
# instruction. Relocation is not supported for that case, so disable
# such usage by requiring word relocations.
PLATFORM_CPPFLAGS += $(call cc-option, -mword-relocations)
PLATFORM_CPPFLAGS += $(call cc-option, -fno-pic)
endif
