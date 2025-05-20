# SPDX-License-Identifier: GPL-2.0
#

ifneq ($(KERNELRELEASE),)

	obj-m += example-IV-01.o
	obj-m += example-IV-02.o
	obj-m += example-IV-03.o
	obj-m += example-IV-04.o
	obj-m += example-IV-05.o
	obj-m += example-IV-06.o
	obj-m += example-IV-07.o
	obj-m += example-IV-08.o

else

KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build
MODULE_DIR := $(shell pwd)

MOD_INSTALL_DIR ?= $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/ild
ARCH ?= $(shell arch)
ifeq ($(ARCH),i686)
	ARCH := x86
else ifeq ($(ARCH),aarch64)
	ARCH := arm64
else ifeq ($(ARCH),armv7l)
	ARCH := arm
else ifeq ($(ARCH),armv6l)
	ARCH := arm
else ifeq ($(ARCH),riscv64)
	ARCH := riscv
endif

USERSPACE  := example-ioctl
USERSPACE  += ioctl-example-IV-06

.PHONY: all

all: modules $(USERSPACE)


.PHONY: modules

modules:
	$(MAKE) -C $(KERNEL_SRC) M=$(MODULE_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)  modules
	rm -f *.o *.mod*

modules_install:
	install -d $(MOD_INSTALL_DIR)
	install -m 0644 example-IV-01.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-IV-02.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-IV-03.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-IV-04.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-IV-05.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-IV-06.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-IV-07.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-IV-08.ko  $(MOD_INSTALL_DIR)/


.PHONY: clean

clean:
	rm -f *.o *.ko *.mod.c .*.o .*.o.d .*.ko .*.mod.c .*.cmd *~ *.ko.unsigned *.mod
	rm -f Module.symvers Module.markers modules.order
	rm -rf .tmp_versions .cache.mk
	rm -f $(USERSPACE)

endif
