# SPDX-License-Identifier: GPL-2.0
#

ifneq ($(KERNELRELEASE),)

	obj-m  = example-I-00.o
	obj-m += example-I-01.o
	obj-m += example-I-02.o
	obj-m += example-I-03.o
else

	KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
	MODULE_DIR := $(shell pwd)

.PHONY: all

all: modules

.PHONY: modules

modules:
	$(MAKE) -C $(KERNEL_DIR) M=$(MODULE_DIR)  modules

.PHONY: clean

clean:
	rm -f *.o *.ko *.mod.c .*.o .*.o.d .*.ko .*.mod.c .*.cmd *~ *.ko.unsigned *.mod
	rm -f Module.symvers Module.markers modules.order
	rm -rf .tmp_versions .cache.mk

endif
