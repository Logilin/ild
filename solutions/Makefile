ifneq (${KERNELRELEASE},)

	#obj-m  = tp-II-02-proc-current-pid.c
	#obj-m += tp-II-03-proc-sum.o
	#obj-m += tp-02-counter.o
	#obj-m += tp-03-messages.o
else

	KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
	MODULE_DIR := $(shell pwd)

.PHONY: all

all: modules

.PHONY: modules

modules:
	${MAKE} -C ${KERNEL_DIR} M=${MODULE_DIR}  modules

.PHONY: clean

clean:
	rm -f *.o *.ko *.mod.c .*.o .*.ko .*.mod.c .*.cmd *~ *.ko.unsigned *.mod
	rm -f Module.symvers Module.markers modules.order
	rm -rf .tmp_versions .cache.mk

endif
