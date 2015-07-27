CC=cc
CFLAGS=-g -Wall -lcrypto -pthread
DEPS=self.h
OBJ=main.o self.o peer.o message.o protocol.o hash.o util.o #file.o client.o
NAME=disponible

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(NAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

test: $(NAME)
	./$(NAME)

clean:
	rm -f $(NAME) *.o
