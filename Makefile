CC=cc
CFLAGS=-g -Wall -lcrypto
DEPS=hash.h
OBJ=file.o hash.o
NAME=disp

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(NAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

test: $(NAME)
	./$(NAME)

clean:
	rm -f $(NAME) *.o
