SHARED_SOURCES := arraylist.c parse.c map.c
SHARED_OBJECTS := $(patsubst %.c,build/%.o,$(SHARED_SOURCES))

DAY_SOURCES = $(wildcard day*.c)
DAY_OBJECTS := $(patsubst %.c,build/%.o,$(DAY_SOURCES))
DAY_EXECUTABLES := $(patsubst %.c,build/%,$(DAY_SOURCES))

CC := gcc
CFLAGS := -std=c17 -Wall -Wextra -pedantic -O3 -march=native -flto -g3 -fwrapv

.SUFFIXES:
.PHONY: all clean

all: $(DAY_EXECUTABLES)

$(DAY_EXECUTABLES): build/%: build/%.o $(SHARED_OBJECTS)
	gcc $(CFLAGS) -o $@ $^

$(SHARED_OBJECTS) $(DAY_OBJECTS): build/%.o: %.c
	@mkdir -p build
	gcc $(CFLAGS) -c -MD -o $@ $<
-include $(SHARED_OBJECTS:.o=.d)
-include $(DAY_OBJECTS:.o=.d)

clean:
	rm -rf build
