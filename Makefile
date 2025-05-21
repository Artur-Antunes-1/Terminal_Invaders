CC = gcc
CFLAGS = -Wall -g -I./include

# Fontes em src/
SRCS = $(wildcard src/*.c)
OBJ  = $(SRCS:.c=.o)
EXEC = terminal_invaders

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

# Compila cada fonte em src/ para objeto em src/
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC) $(OBJ)