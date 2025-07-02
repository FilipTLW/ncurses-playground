CFLAGS?=-Wall -O2
CFLAGS:=$(CFLAGS) -lncurses

OBJS=\
src/main.o

.SUFFIXES: .o .c

all: out/ncurses-playground

out/ncurses-playground: $(OBJS)
	mkdir -p out
	$(CC) $< -o out/ncurses-playground $(CFLAGS)

.c.o:
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf out
	rm -f $(OBJS)
