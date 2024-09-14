CC := clang

# Note: run `export LD_PRELOAD=$(clang -print-file-name=libclang_rt.asan-x86_64.so)` if you want to use Address Sanitizer
ASAN_FLAGS ?= #-fsanitize=address -shared-libasan

# compiler flags
DEBUG     := -ggdb -gdwarf-4 -ferror-limit=1 $(ASAN_FLAGS)
OPTIMISE  := -O0
WARNING   := -Wall -Wextra -Wno-variadic-macros \
             -Wno-overlength-strings -pedantic
EXTRA     := -std=c11 -fPIC
CFLAGS    := $(DEBUG) $(OPTIMISE) $(WARNING) $(EXTRA)
DFLAGS    ?= # -DDEBUG

# library dependencies
SUBLIBFLAGS := $(shell git submodule foreach --recursive --quiet 'echo -L$$displaypath')
SUBLIBFLAGS += -lstack
SUBLIBS := libstack.a

# linker flags

LDFLAGS := $(ASAN_FLAGS) -L. -ltrycatch $(SUBLIBFLAGS)

# commands

COMPILE:=$(CC) $(CFLAGS) $(DFLAGS)

# targets

lib: libtrycatch.a

tests: build $(ALL_OBJ_FILES) test_error test_try_catch

SRC_C_FILES := $(wildcard src/*.c)
SRC_H_FILES := $(wildcard src/*.h)
SRC_OBJ_FILES := $(patsubst src/%.c, build/src/%.o, $(SRC_C_FILES))

TESTS_C_FILES := $(wildcard tests/*.c)
TESTS_H_FILES := $(wildcard tests/*.h)
TESTS_OBJ_FILES := $(patsubst tests/%.c, build/tests/%.o, $(TESTS_C_FILES))

ALL_C_FILES := $(SRC_C_FILES) $(TESTS_C_FILES)
ALL_H_FILES := $(SRC_H_FILES) $(TESTS_H_FILES)
ALL_OBJ_FILES := $(SRC_OBJ_FILES) $(TESTS_OBJ_FILES)

ALL_CODE_FILES := $(ALL_C_FILES) $(ALL_H_FILES)
# target files

libtrycatch.a: $(SRC_OBJ_FILES) $(SUBLIBS)
	ar rcs libtrycatch.a $(SRC_OBJ_FILES)

build/src/%.o: src/%.c $(SRC_C_FILES) $(SRC_H_FILES) | build
	$(COMPILE) -c -o $@ $<

build/tests/%.o: tests/%.c $(TESTS_C_FILES) $(TESTS_H_FILES) | build
	$(COMPILE) -c -o $@ $<

test_error: build/tests/custom_error.o $(SUBLIBS)
	$(COMPILE) -o $@ tests/test_error.c build/tests/custom_error.o $(LDFLAGS)

test_try_catch:
	$(COMPILE) -o $@ tests/test_try_catch.c build/tests/custom_error.o $(LDFLAGS)

libstack.a:
	$(MAKE) -C c-lib-stack libstack.a

# target directories

build:
	mkdir -p build
	mkdir -p build/src
	mkdir -p build/tests

# phony targets

install: libtrycatch.a src/trycatch.h
	install -d /usr/local/lib /usr/local/include
	# Octal permissions: 6 = rw-, 4 = r--
	install -m 644 libtrycatch.a /usr/local/lib
	install -m 644 src/trycatch.h /usr/local/include

uninstall:
	rm -f /usr/local/lib/libtrycatch.a /usr/local/include/trycatch.h

clean:
	rm -f test_error test_try_catch
	rm -rf build
	$(MAKE) -C c-lib-stack clean

.PHONY: install uninstall clean tests lib