NAME=dsp

CPPFLAGS=
CFLAGS=-std=c11 -Wall -Wno-parentheses

CLIENT_SRCS=client file
CLIENT_SRCS:=$(addprefix client/, $(CLIENT_SRCS:%=%.c))
CLIENT_OBJ:=$(CLIENT_SRCS:%.c=%.o)

SRCS=self config crypto nodes msg net
SRCS:=$(SRCS:%=%.c)
OBJ:=$(SRCS:%.c=%.o)

debug: CFLAGS+=-g
debug: all

nodebug: CPPFLAGS+=-DNDEBUG
nodebug: all

all: $(NAME) 

$(CLIENT_OBJ): lib$(NAME).h client/client.h $(CLIENT_SRCS)
	cd client && \
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(notdir $(CLIENT_SRCS))

$(OBJ): self.h $(SRCS)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(SRCS)

$(NAME): lib$(NAME).so $(CLIENT_OBJ)
	$(CC) -o $(NAME) lib$(NAME).so $(CLIENT_OBJ)

lib$(NAME).so: lib$(NAME).h api.c $(OBJ)
	$(CC) -shared -fpic -o lib$(NAME).so api.c $(OBJ)

clean:
	rm -f $(NAME) lib$(NAME).so $(CLIENT_OBJ) $(OBJ)
