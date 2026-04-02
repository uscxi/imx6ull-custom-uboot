DEVICE_TREE ?= $(CONFIG_DEFAULT_DEVICE_TREE:"%"=%)
ifeq ($(DEVICE_TREE),)
DEVICE_TREE := unset
endif

dt_dir := arch/$(ARCH)/dts

DTB := $(dt_dir)/$(DEVICE_TREE).dtb

$(obj)/dt.dtb: $(DTB) FORCE
	$(call if_changed,shipped)

targets += dt.dtb

$(DTB): arch-dtbs
	$(Q)test -e $@ || (						\
	echo >&2;							\
	echo >&2 "Device Tree Source ($@) is not correctly specified.";	\
	echo >&2 "Please define 'CONFIG_DEFAULT_DEVICE_TREE'";		\
	echo >&2 "or build with 'DEVICE_TREE=<device_tree>' argument";	\
	echo >&2;							\
	/bin/false)

PHONY += arch-dtbs
arch-dtbs:
	$(Q)$(MAKE) $(build)=$(dt_dir) dtbs

obj-$(CONFIG_OF_EMBED) := dt.dtb.o

# Target for U-Boot proper
dtbs: $(obj)/dt.dtb

clean-files := dt.dtb.S

# Let clean descend into dts directories
subdir- += ../arch/arm/dts
