CC=cc
CFLAGS=-g -Wall -lcrypto
DEPS=file.h hash.h peer.h self.h
OBJ=file.o hash.o peer.o self.o
NAME=disp

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(NAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

test: $(NAME)
	./$(NAME)

clean:
	rm -f $(NAME) *.o
