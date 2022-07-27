ifneq ($(KERNELRELEASE),)

	# obj-m  = tp-I-05-messages.o
	# obj-m += tp-II-02-proc-current-pid.o
	# obj-m += tp-II-03-proc-sum.o
	# obj-m += tp-02-counter.o
	# obj-m += tp-03-messages.o
	# obj-m += tp-III-03-memory-addresses.o
else

	KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
	MODULE_DIR := $(shell pwd)

	# EXE_FILES = tp-II-01-hello-world


.PHONY: all
all: modules userspace


.PHONY: modules
modules:
	$(MAKE) -C $(KERNEL_DIR) M=$(MODULE_DIR)  modules


.PHONY: userspace
userspace: $(EXE_FILES)

tp-II-01-hello-world: tp-II-01-hello-world.c
	$(CC) -Wall -o $@ $<


.PHONY: clean
clean:
	rm -f *.o *.ko *.mod.c .*.o .*.ko .*.mod.c .*.cmd *~ *.ko.unsigned *.mod
	rm -f Module.symvers Module.markers modules.order
	rm -rf .tmp_versions .cache.mk
	rm -f $(EXE_FILES)

endif
