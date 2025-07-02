CFLAGS?=-Wall -O2
CFLAGS:=$(CFLAGS) -lncurses

OBJS=\
src/main.o

.SUFFIXES: .o .c

all: ncurses-playground

ncurses-playground: $(OBJS)
	$(CC) $< -o $@ $(CFLAGS)

.c.o:
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJS)
