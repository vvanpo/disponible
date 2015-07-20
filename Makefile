CC=cc
CFLAGS=-g -Wall -lcrypto
OBJ=file.o hash.o
NAME=disp

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(NAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

test: $(NAME)
	./$(NAME)

clean:
	rm -f $(NAME) *.o
