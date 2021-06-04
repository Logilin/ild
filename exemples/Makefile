# SPDX-License-Identifier: GPL-2.0
#

SUBDIRS	= $(wildcard *)

.PHONY:all
all:
	@set -e;                \
	for dir in $(SUBDIRS);  \
	do                      \
		if [ -d $$dir ]; then \
		cd $$dir;             \
		make;                 \
		cd -;                 \
		fi;                   \
	done

.PHONY: clean
clean:
	@set -e;                \
	for dir in $(SUBDIRS);  \
	do                      \
		if [ -d $$dir ]; then \
		cd $$dir;             \
		make clean;           \
		cd -;                 \
		fi;                   \
	done

