obj-y += init/

obj-y += main.o
obj-y += version.o
# # boards
obj-y += board_f.o

obj-$(CONFIG_$(PHASE_)SERIAL) += console.o

obj-y += dlmalloc.o
obj-$(CONFIG_$(PHASE_)SYS_MALLOC_F) += malloc_simple.o
obj-$(CONFIG_$(PHASE_)LOG) += log.o
