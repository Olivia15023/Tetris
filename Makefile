CC = gcc
CFLAGS = -Wall -Iheader
LDFLAGS = -lncurses
SRC = cod/tetris.c cod/game.c
OBJ = $(SRC:.c=.o)
EXEC = tetris
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)


cod/%.o: cod/%.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(OBJ) $(EXEC)