NAME=dsp

CFLAGS=-std=c11 -Wall -Wno-parentheses

debug: CFLAGS+=-g
debug: all

nodebug: CPPFLAGS+=-DNDEBUG
nodebug: all

all: dsp server_api

dsp: 
	cd client && \
	$(CC) -c $(CFLAGS) $(CPPFLAGS) *.c && \
	$(CC) -o $(NAME) *.o -ldsp && \
	mv $(NAME) ../

server_api: server_api/libdsp.so
	cd server_api; \
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -fpic *.c; \
	$(CC) -shared -fpic -o libdsp.so *.o

clean:
	rm -f $(NAME) client/*.o server_api/*.o server_api/libdsp.so
