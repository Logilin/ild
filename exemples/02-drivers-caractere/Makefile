ifneq (${KERNELRELEASE},)

	obj-m  = exemple_01.o
	obj-m += exemple_02.o
	obj-m += exemple_03.o
	obj-m += exemple_04.o
	obj-m += exemple_05.o
	obj-m += exemple_06.o
#	obj-m += exemple_07.o
#	obj-m += exemple_09.o
#	obj-m += exemple_18.o
#	obj-m += exemple_19.o
#	obj-m += exemple_20.o

ifneq (${RPI},)
#	obj-m += exemple_08_A.o
#	obj-m += exemple_10_A.o
#	obj-m += exemple_11_A.o
#	obj-m += exemple_12_A.o
#	obj-m += exemple_13_A.o
#	obj-m += exemple_14_A.o
#	obj-m += exemple_15_A.o
#	obj-m += exemple_16_A.o
#	obj-m += exemple_17_A.o
else
#	obj-m += exemple_08_B.o
#	obj-m += exemple_10_B.o
#	obj-m += exemple_11_B.o
#	obj-m += exemple_12_B.o
#	obj-m += exemple_13_B.o
#	obj-m += exemple_14_B.o
#	obj-m += exemple_15_B.o
#	obj-m += exemple_16_B.o
#	obj-m += exemple_17_B.o
endif

#	obj-m += exemple_10_C.o
#	obj-m += exemple_11_C.o
#	obj-m += exemple_12_C.o
#	obj-m += exemple_13_C.o
#	obj-m += exemple_14_C.o
#	obj-m += exemple_15_C.o
#	obj-m += exemple_16_C.o
#	obj-m += exemple_17_C.o

else

	KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
	MODULE_DIR := $(shell pwd)

	CROSS_KERNEL_DIR ?= /usr/local/src/linux-rpi-3.6.11
	CROSS_ARCH       ?= arm
	CROSS_COMPILE    ?= /usr/local/cross/rpi/bin/arm-linux-

	USER_PROGS :=                \
	          exemple_ioctl      \
	          ioctl_exemple_06   \
#                 cat_nonblock       \
#                 test_select        \
#                 test_mmap          \



	RPI := 1
	
	CFLAGS := -Wall -g

all: user modules

modules:
	${MAKE} -C ${KERNEL_DIR} SUBDIRS=${MODULE_DIR}  RPI=${RPI} modules

cross:
	${MAKE} -C ${CROSS_KERNEL_DIR} SUBDIRS=${MODULE_DIR} ARCH=${CROSS_ARCH} CROSS_COMPILE=${CROSS_COMPILE} RPI=${RPI} modules

user: ${USER_PROGS}


clean:
	rm -f *.o *.ko *.mod.c .*.o .*.ko .*.mod.c .*.cmd *~
	rm -f Module.symvers Module.markers modules.order
	rm -rf .tmp_versions
	rm -f ${USER_PROGS}

endif
