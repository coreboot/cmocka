.PHONY: check-gcc

# Part of csdiff package on Fedora
CSGREP ?= csgrep

CFLAGS ?= -Wall -Wno-uninitialized -Wno-unused-variable -Wno-unused-but-set-variable -Werror

check-gcc:
	gcc -o /dev/null -fPIC -shared -include ./coverity-builtins.h $(CFLAGS) *.c 2> >($(CSGREP))
