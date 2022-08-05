ifneq ($(KERNELRELEASE),)

	# obj-m += tp-I-05-messages.o
	# obj-m += tp-II-02-proc-current-pid.o
	# obj-m += tp-II-03-proc-sum.o
	# obj-m += tp-III-01-timing.o
	# obj-m += tp-III-02-log.o
	# obj-m += tp-III-03-memory-addresses.o
	# obj-m += tp-IV-01-counter-read.o
	# obj-m += tp-IV-02-counter-write.o
	# obj-m += tp-IV-03-counter-ioctl.o
	# obj-m += tp-V-01-message-read-write.o
	# obj-m += tp-V-02-message-mutex.o
	# obj-m += tp-V-03-blocking-read.o
	# obj-m += tp-V-04-blocking-write.o
else

	KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
	MODULE_DIR := $(shell pwd)

	# EXE_FILES += tp-II-01-hello-world
	# EXE_FILES += set-step


.PHONY: all
all: modules userspace


.PHONY: modules
modules:
	$(MAKE) -C $(KERNEL_DIR) M=$(MODULE_DIR)  modules


.PHONY: userspace
userspace: $(EXE_FILES)

tp-II-01-hello-world: tp-II-01-hello-world.c
	$(CC) -Wall -o $@ $<

set-step: set-step.c tp-IV-03-counter-ioctl.h
	$(CC) -Wall -o $@ $<

.PHONY: clean
clean:
	rm -f *.o *.ko *.mod.c .*.o .*.ko .*.mod.c .*.cmd *~ *.ko.unsigned *.mod
	rm -f Module.symvers Module.markers modules.order
	rm -rf .tmp_versions .cache.mk
	rm -f $(EXE_FILES)

endif
