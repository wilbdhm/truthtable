CC = gcc
CFLAGS = -std=c11 -Wall -Wpedantic
LFLAGS =
HDRDIR = hdr
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c,obj/%.o,$(SRCS))

truthtable: $(OBJS)
	$(CC) -o $@ $^

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) -o $@ $^ -I$(HDRDIR)

.PHONY: build debug clean release
build: truthtable

debug: CFLAGS += -g
debug: build

release: CFLAGS += -O2
release: build

clean:
	${RM} truthtable obj/*.o
