ifneq (${KERNELRELEASE},)

	obj-m  = exemple_01.o
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
	obj-m += exemple_14.o
	obj-m += exemple_15.o
	obj-m += exemple_16.o
	obj-m += exemple_17.o
	obj-m += exemple_18.o
	obj-m += exemple_19.o
	obj-m += exemple_20.o
	obj-m += exemple_21.o


else


	# Uncomment and complete the following lines if you cross-compile the examples.
	# export ARCH          := arm
	# export CROSS_COMPILE := arm-linux-
	# export KERNEL_DIR    := ~/cross/kernel/linux-rpi-3.6.11


	KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
	MODULE_DIR := $(shell pwd)

	USER_PROGS :=                \
	          exemple_ioctl      \
	          ioctl_exemple_06   \
	          cat_nonblock       \
	          test_select        \
	          test_mmap          \

.PHONY: all
all: user modules

.PHONY: modules
modules:
	${MAKE} -C ${KERNEL_DIR} SUBDIRS=${MODULE_DIR}  modules

.PHONY: user
user: ${USER_PROGS}

%:%.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean,
clean:
	rm -f *.o *.ko *.mod.c .*.o .*.ko .*.mod.c .*.cmd *~ *.ko.unsigned
	rm -f Module.symvers Module.markers modules.order
	rm -rf .tmp_versions
	rm -f ${USER_PROGS}

endif

