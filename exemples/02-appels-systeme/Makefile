# SPDX-License-Identifier: GPL-2.0
#

ifneq ($(KERNELRELEASE),)

	obj-m += example-II-01.o
	obj-m += example-II-02.o
	obj-m += example-II-03.o
	obj-m += example-II-04.o
	obj-m += example-II-05.o
	obj-m += example-II-06.o
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


.PHONY: all

all: modules


.PHONY: modules

modules:
	$(MAKE) -C $(KERNEL_SRC) M=$(MODULE_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)  modules


.PHONY: modules_install

modules_install:
	install -d $(MOD_INSTALL_DIR)
	install -m 0644 example-II-01.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-II-02.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-II-03.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-II-04.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-II-05.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-II-06.ko  $(MOD_INSTALL_DIR)/


.PHONY: clean

clean:
	rm -f *.o *.ko *.mod.c .*.o .*.o.d .*.ko .*.mod.c .*.cmd *~ *.ko.unsigned *.mod
	rm -f Module.symvers Module.markers modules.order
	rm -rf .tmp_versions .cache.mk

endif
