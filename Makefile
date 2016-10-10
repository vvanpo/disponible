NAME=dsp

CFLAGS=-std=c11 -Wall -Wno-parentheses

CLIENT_OBJ=
CLIENT_OBJ:=$(addprefix client/, $(CLIENT_OBJ))
SERVER_OBJ=self.o node.o file.o crypto.o
SERVER_OBJ:=$(addprefix server/, $(SERVER_OBJ))

debug: CFLAGS+=-g
debug: all

nodebug: CPPFLAGS+=-DNDEBUG
nodebug: all

all: $(NAME) 

$(CLIENT_OBJ): lib$(NAME).h server.h client/*.c
	cd client && \
	$(CC) -c $(CFLAGS) $(CPPFLAGS) *.c

$(SERVER_OBJ): lib$(NAME).h server.h server/self.h server/*.c
	cd server && \
	$(CC) -c $(CFLAGS) $(CPPFLAGS) *.c

$(NAME): lib$(NAME).so $(CLIENT_OBJ) $(SERVER_OBJ)
	$(CC) -o $(NAME) lib$(NAME).so $(CLIENT_OBJ) $(SERVER_OBJ)

lib$(NAME).so: lib$(NAME).h api.c
	$(CC) -shared -fpic -o lib$(NAME).so api.c

clean:
	rm -f $(NAME) lib$(NAME).so $(CLIENT_OBJ) $(SERVER_OBJ)
