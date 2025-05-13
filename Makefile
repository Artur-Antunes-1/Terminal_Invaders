CC      = gcc
CFLAGS  = -I./libs/cli-lib -I./include -Wall -Wextra -std=c11
LDFLAGS =

SRC     = src/main.c src/game.c src/input.c src/draw.c src/score.c
OBJ     = $(SRC:.c=.o)
TARGET  = invaders

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)
