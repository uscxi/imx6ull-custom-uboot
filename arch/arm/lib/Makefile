lib-$(CONFIG_USE_PRIVATE_LIBGCC) += ashldi3.o ashrdi3.o lshrdi3.o \
				    lib1funcs.o uldivmod.o div0.o \
				    div64.o muldi3.o

obj-y	+= vectors.o crt0.o

obj-$(CONFIG_$(PHASE_)USE_ARCH_MEMSET) += memset.o


# some files can only build in ARM or THUMB2, not THUMB1

ifdef CONFIG_SYS_THUMB_BUILD
asflags-$(CONFIG_HAS_THUMB2) += -DCONFIG_THUMB2_KERNEL
ifndef CONFIG_HAS_THUMB2

# For .S, drop -mthumb* and other thumb-related options.
# CFLAGS_REMOVE_* would not have an effet, so AFLAGS_REMOVE_*
# was implemented and is used here.
# Also, define ${target}_NO_THUMB_BUILD for these two targets
# so that the code knows it should not use Thumb.

AFLAGS_REMOVE_memset.o := -mthumb -mthumb-interwork
AFLAGS_REMOVE_memcpy.o := -mthumb -mthumb-interwork
AFLAGS_memset.o := -DMEMSET_NO_THUMB_BUILD
AFLAGS_memcpy.o := -DMEMCPY_NO_THUMB_BUILD

endif
endif
