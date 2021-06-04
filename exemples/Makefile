# SPDX-License-Identifier: GPL-2.0
#

SUBDIRS	= $(wildcard *)

.PHONY:all
all:
	@set -e;                \
	for i in $(SUBDIRS);    \
	do                      \
		if [ -d $$i ]; then \
		cd $$i;             \
		make;               \
		cd -;               \
		fi;                 \
	done

.PHONY: clean
clean:
	@set -e;                \
	for i in $(SUBDIRS);    \
	do                      \
		if [ -d $$i ]; then \
		cd $$i;             \
		make clean;         \
		cd -;               \
		fi;                 \
	done

