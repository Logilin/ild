# SPDX-License-Identifier: GPL-2.0
#

ifneq ($(KERNELRELEASE),)

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
	obj-m += example-V-11.o
	obj-m += example-select.o

else

KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build
MODULE_DIR := $(shell pwd)
MOD_INSTALL_DIR ?= $(DESTDIR)/lib/modules/$(KERNEL_VERSION)/ild
ARCH ?= $(shell arch)

USER_PROGS :=    cat-non-block        \
                 test-mmap            \
                 test-select          \

.PHONY: all

all: user modules


.PHONY: modules

modules:
	$(MAKE) -C $(KERNEL_SRC) M=$(MODULE_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)  modules


.PHONY: modules_install

modules_install:
	install -d $(MOD_INSTALL_DIR)
	install -m 0644 example-V-01.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-V-02.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-V-03.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-V-04.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-V-05.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-V-06.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-V-07.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-V-08.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-V-09.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-V-10.ko  $(MOD_INSTALL_DIR)/
	install -m 0644 example-V-11.ko  $(MOD_INSTALL_DIR)/


.PHONY: user

user: $(USER_PROGS)


%:%.c
	$(CC) $(CFLAGS) $(LDFLAGS)  -o $@ $<


.PHONY: clean
clean:
	rm -f *.o *.ko *.mod.c .*.o .*.o.d .*.ko .*.mod.c .*.cmd *~ *.ko.unsigned *.mod
	rm -f Module.symvers Module.markers modules.order
	rm -rf .tmp_versions .cache.mk
	rm -f $(USER_PROGS)

endif

