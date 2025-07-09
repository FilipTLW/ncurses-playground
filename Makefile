PKG_CFLAGS:=$(shell pkg-config --cflags ncurses)
PKG_LDFLAGS:=$(shell pkg-config --libs ncurses)

CFLAGS?=-Wall -O2
CFLAGS+= $(PKG_CFLAGS)
LDFLAGS?=
LDFLAGS+= $(PKG_LDFLAGS)

OBJS=\
src/main.o\
src/file_dialog.o

.SUFFIXES: .o .c

all: out/ncurses-playground

out/ncurses-playground: $(OBJS)
	mkdir -p out
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf out
	rm -f $(OBJS)
