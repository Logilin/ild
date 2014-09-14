SUBDIRS	= $(wildcard *)

all:
	@set -e;                \
	for i in $(SUBDIRS);    \
	do                      \
		if [ -d $$i ]; then	\
		cd $$i;             \
		make;               \
		cd -;               \
		fi;                 \
	done

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

