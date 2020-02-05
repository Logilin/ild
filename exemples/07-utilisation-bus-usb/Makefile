ifneq (${KERNELRELEASE},)

	obj-m += exemple-VII-01.o
	obj-m += exemple-VII-02.o
	obj-m += exemple-VII-03.o
	obj-m += exemple-VII-04.o
	obj-m += exemple-VII-05.o

else

	KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
	MODULE_DIR := $(shell pwd)

	CFLAGS := -Wall -g

all: modules

modules:
	${MAKE} -C ${KERNEL_DIR} M=${MODULE_DIR}  modules


clean:
	rm -f *.o *.ko *.mod.c .*.o .*.ko .*.mod.c .*.cmd *~ *.ko.unsigned
	rm -f Module.symvers Module.markers modules.order
	rm -rf .tmp_versions .cache.mk

endif
