CC = gcc
CFLAGS = -Wall -Iheader $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_ttf
SRC = cod/tetris.c cod/game.c cod/screen.c
OBJ = $(SRC:.c=.o)
EXEC = tetris

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

cod/%.o: cod/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)