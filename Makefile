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

# dependencies
DEPS := c-lib-stack/libstack.a

# linker flags

LDFLAGS := $(ASAN_FLAGS) -L. -ltrycatch

# commands

COMPILE:=$(CC) $(CFLAGS) $(DFLAGS)

# files

CODE_FILES := $(wildcard src/*.c src/*.h tests/*.c tests/*.h)
SRC_OBJ_FILES := $(patsubst src/%.c, build/src/%.o, $(SRC_C_FILES))
TESTS_OBJ_FILES := $(patsubst tests/%.c, build/tests/%.o, $(TESTS_C_FILES))

# targets

libtrycatch.a: $(SRC_OBJ_FILES) sublib-objs
	ar rcs libtrycatch.a $(SRC_OBJ_FILES) build/sublib-objs/*.o

sublib-objs: | build
	$(MAKE) -C c-lib-stack libstack.a
	ar x c-lib-stack/libstack.a --output build/sublib-objs

build:
	mkdir -p build/sublib-objs
	mkdir -p build/src
	mkdir -p build/tests

build/src/%.o: src/%.c | build
	$(COMPILE) -c -o $@ $<

build/tests/%.o: tests/%.c $(CODE_FILES) | build
	$(COMPILE) -c -o $@ $<

test_error test_try_catch: build/tests/custom_error.o libtrycatch.a
	$(COMPILE) -o $@ tests/$@.c build/tests/custom_error.o $(LDFLAGS)

tests: test_error test_try_catch

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

.PHONY: install uninstall clean tests lib sublib-objs