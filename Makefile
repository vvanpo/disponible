CC=cc
CFLAGS=-g -Wall -lcrypto -pthread
DEPS=file.h hash.h peer.h self.h util.h
OBJ=main.o file.o hash.o peer.o self.o message.o util.o client.o
NAME=disp

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(NAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

test: $(NAME)
	./$(NAME)

clean:
	rm -f $(NAME) *.o
