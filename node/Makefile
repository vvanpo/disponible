CFLAGS=-std=c11 -Wall -Wno-parentheses
DEBUG=-g
NODEBUG=-DNDEBUG

DEPS=node.h conf.h peer.h cryp.h prot.h
OBJ=node.o conf.o peer.o cryp.o prot.o

all: CFLAGS+=$(DEBUG)
all: daemon

no-debug: CFLAGS+=$(NODEBUG)
no-debug: daemon

daemon: $(OBJ)
	cc -o daemon main.c $^ $(CFLAGS) -lcrypto

%.o: %.c $(DEPS)
	cc -c $< $(CFLAGS)

clean:
	rm -f daemon *.o
