CFLAGS += -std=c99 -Wall -Wextra
PREFIX ?= /usr
BINDIR ?= $(PREFIX)/bin
CC     ?= gcc

all: bwm

bwm: bwm.c
	$(CC) -O3 $(CFLAGS) -o $@ $< -lX11 $(LDFLAGS)

clean:
	rm -f bwm
