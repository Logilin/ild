ifneq (${KERNELRELEASE},)

	obj-m += exemple-4-01.o
	obj-m += exemple-4-02.o
	obj-m += exemple-4-03.o
	obj-m += exemple-4-04.o
	obj-m += exemple-4-05.o
	obj-m += exemple-4-06.o
	obj-m += exemple-4-07.o
	obj-m += exemple-4-08.o

else

	KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
	MODULE_DIR := $(shell pwd)

	USERSPACE  := exemple-ioctl
	USERSPACE  += ioctl-exemple-4-06

.PHONY: all

all: modules $(USERSPACE)

.PHONY: modules

modules:
	${MAKE} -C ${KERNEL_DIR} SUBDIRS=${MODULE_DIR}  modules

.PHONY: clean

clean:
	rm -f *.o *.ko *.mod.c .*.o .*.ko .*.mod.c .*.cmd *~ *.ko.unsigned
	rm -f Module.symvers Module.markers modules.order
	rm -rf .tmp_versions
	rm -f $(USERSPACE)

endif
