ifneq (${KERNELRELEASE},)

	obj-m += exemple-III-01.o
	obj-m += exemple-III-02.o
	obj-m += exemple-III-03.o
	obj-m += exemple-III-04.o
	obj-m += exemple-III-05.o
	obj-m += exemple-III-06.o

else

	KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
	MODULE_DIR := $(shell pwd)

.PHONY: all

all: modules

.PHONY: modules

modules:
	${MAKE} -C ${KERNEL_DIR} SUBDIRS=${MODULE_DIR}  modules

.PHONY: clean

clean:
	rm -f *.o *.ko *.mod.c .*.o .*.ko .*.mod.c .*.cmd *~ *.ko.unsigned
	rm -f Module.symvers Module.markers modules.order
	rm -rf .tmp_versions

endif