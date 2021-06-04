# SPDX-License-Identifier: GPL-2.0
#

ifneq (${KERNELRELEASE},)

	obj-m  = example-V-01.o
	obj-m += example-V-02.o
	obj-m += example-V-03.o
	obj-m += example-V-04.o
	obj-m += example-V-05.o
	obj-m += example-V-06.o
	obj-m += example-V-07.o
	obj-m += example-V-08.o
	obj-m += example-V-09.o
	obj-m += example-V-10.o

else


	# Uncomment and complete the following lines if you cross-compile the examples.
	# export ARCH          := arm
	# export CROSS_COMPILE := arm-linux-
	# export KERNEL_DIR    := ~/cross/kernel/linux-rpi-3.6.11

	KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
	MODULE_DIR := $(shell pwd)

	USER_PROGS :=    cat-non-block        \
	                 test-mmap            \

.PHONY: all
all: user modules

.PHONY: modules
modules:
	${MAKE} -C ${KERNEL_DIR} M=${MODULE_DIR}  modules

.PHONY: user
user: ${USER_PROGS}

%:%.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS)  -o $@ $<

.PHONY: clean,
clean:
	rm -f *.o *.ko *.mod.c .*.o .*.o.d .*.ko .*.mod.c .*.cmd *~ *.ko.unsigned *.mod
	rm -f Module.symvers Module.markers modules.order
	rm -rf .tmp_versions .cache.mk
	rm -f ${USER_PROGS}

endif

