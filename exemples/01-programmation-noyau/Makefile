ifneq (${KERNELRELEASE},)

	obj-m  = exemple_00.o
	obj-m += exemple_01.o
	obj-m += exemple_02.o
	obj-m += exemple_03.o
	obj-m += exemple_04.o
	obj-m += exemple_05.o
	obj-m += exemple_06.o
	obj-m += exemple_07.o
	obj-m += exemple_08.o
	obj-m += exemple_09.o
	obj-m += exemple_10.o
	obj-m += exemple_11.o
	obj-m += exemple_12.o
	obj-m += exemple_13.o
else

	KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
	MODULE_DIR := $(shell pwd)

all: modules

modules:
	${MAKE} -C ${KERNEL_DIR} SUBDIRS=${MODULE_DIR}  modules

clean:
	rm -f *.o *.ko *.mod.c .*.o .*.ko .*.mod.c .*.cmd *~ *.ko.unsigned
	rm -f Module.symvers Module.markers modules.order
	rm -rf .tmp_versions

endif
