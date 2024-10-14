CC=gcc 					#specifica il compilatore da usare
CFLAGS="-Wall -std=c11"	#parametro utilizzato dal compilatore C
SRC = $(wildcard *.c)
BIN = $(SRC:.c=)

all: $(BIN)

%: %.c
	$(CC) $(CFLAG) -o $@ $<