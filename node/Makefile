CFLAGS=-std=c11 -Wall -Wno-parentheses
DEBUG=-g
NODEBUG=-DNDEBUG

DEPS=node.h conf.h peer.h cryp.h
OBJ=node.o conf.o peer.o cryp.o prot.o

daemon: $(OBJ)
	cc -o daemon $^ $(CFLAGS)

%.o: %.c $(DEPS)
	cc -c -o $@ $< $(CFLAGS)

clean:
	rm -f daemon *.o
