ifneq (${KERNELRELEASE},)

	obj-m  = exemple-5-01.o
	obj-m += exemple-5-02.o
	obj-m += exemple-5-03.o
	obj-m += exemple-5-04.o
	obj-m += exemple-5-05.o
	obj-m += exemple-5-06.o
	obj-m += exemple-5-07.o

else


	# Uncomment and complete the following lines if you cross-compile the examples.
	# export ARCH          := arm
	# export CROSS_COMPILE := arm-linux-
	# export KERNEL_DIR    := ~/cross/kernel/linux-rpi-3.6.11

	KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
	MODULE_DIR := $(shell pwd)

	USER_PROGS :=                \

.PHONY: all
all: user modules

.PHONY: modules
modules:
	${MAKE} -C ${KERNEL_DIR} SUBDIRS=${MODULE_DIR}  modules

.PHONY: user
user: ${USER_PROGS}

%:%.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS)  -o $@ $<

.PHONY: clean,
clean:
	rm -f *.o *.ko *.mod.c .*.o .*.ko .*.mod.c .*.cmd *~ *.ko.unsigned
	rm -f Module.symvers Module.markers modules.order
	rm -rf .tmp_versions
	rm -f ${USER_PROGS}

endif

