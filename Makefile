CPPFLAGS=
CFLAGS=-Wall -Wno-parentheses

CLIENT_SRCS=client file
CLIENT_SRCS:=$(addprefix client/, $(CLIENT_SRCS:%=%.c))
CLIENT_OBJ:=$(CLIENT_SRCS:%.c=%.o)

SRCS=dsp db crypto
SRCS:=$(SRCS:%=%.c)
OBJ:=$(SRCS:%.c=%.o)

debug: CFLAGS+=-g
debug: all

nodebug: CPPFLAGS+=-DNDEBUG
nodebug: all

all: dsp

$(CLIENT_OBJ): libdsp.h client/client.h $(CLIENT_SRCS)
	cd client && \
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(notdir $(CLIENT_SRCS))

$(OBJ): dsp.h $(SRCS)
	$(CC) -fpic -c $(CFLAGS) $(CPPFLAGS) $(SRCS)

dsp: libdsp.so $(CLIENT_OBJ)
	$(CC) -o dsp libdsp.so $(CLIENT_OBJ)

libdsp.so: libdsp.h $(OBJ)
	$(CC) -shared -fpic -o libdsp.so $(OBJ)

clean:
	rm -f dsp libdsp.so $(CLIENT_OBJ) $(OBJ)
