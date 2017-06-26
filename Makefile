CPPFLAGS=
CFLAGS=-Wall -Wpedantic -Wno-parentheses -Wno-missing-braces

CLIENT_SRCS=client
CLIENT_SRCS:=$(addprefix client/, $(CLIENT_SRCS:%=%.c))
CLIENT_OBJ:=$(CLIENT_SRCS:%.c=%.o)

SRCS=dsp error db crypto net
SRCS:=$(SRCS:%=%.c)
OBJ:=$(SRCS:%.c=%.o)

debug: CFLAGS+=-g
debug: all

nodebug: CPPFLAGS+=-DNDEBUG
nodebug: CFLAGS+=-O2
nodebug: all

all: dsp

$(CLIENT_OBJ): libdsp.h client/client.h $(CLIENT_SRCS)
	cd client && \
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(notdir $(CLIENT_SRCS))

dsp: libdsp.so $(CLIENT_OBJ)
	$(CC) -L. -o dsp $(CLIENT_OBJ) -ldsp

$(OBJ): dsp.h $(SRCS)
	$(CC) -fpic -pthread -c $(CFLAGS) $(CPPFLAGS) $(SRCS)

libdsp.so: libdsp.h $(OBJ)
	$(CC) -shared -pthread -o libdsp.so $(OBJ) -lm -lsqlite3 -l:libnacl.a -l:randombytes.o

clean:
	rm -f dsp libdsp.so $(CLIENT_OBJ) $(OBJ)

run: export LD_LIBRARY_PATH=.
run: dsp
	./dsp

prefix=/usr/local
install: dsp libdsp.so libdsp.h
	install -m644 libdsp.h $(prefix)/include/dsp.h
	install libdsp.so $(prefix)/lib
	install dsp $(prefix)/bin


