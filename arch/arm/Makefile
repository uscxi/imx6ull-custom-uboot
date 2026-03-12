
# This selects which instruction set is used.
arch-$(CONFIG_CPU_V7A)		=$(call cc-option, -march=armv7-a, \
				 $(call cc-option, -march=armv7))

arch-y += -D__LINUX_ARM_ARCH__=$(CONFIG_SYS_ARM_ARCH)

# Evaluate arch cc-option calls now
arch-y := $(arch-y)

# This selects how we optimise for the processor.

tune-$(CONFIG_CPU_V7A)		=-mtune=generic-armv7-a

# Evaluate tune cc-option calls now
tune-y := $(tune-y)

PLATFORM_CPPFLAGS += $(arch-y) $(tune-y)

head-y := arch/arm/cpu/$(CPU)/start.o
libs-y += arch/arm/cpu/$(CPU)/
libs-y += arch/arm/cpu/
