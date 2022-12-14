CFLAGS=-g -Werror -Wall `pkg-config allegro-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5  --cflags`
LFLAGS=`pkg-config allegro-5 allegro_font-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 --libs` -lm
CC=gcc
SRC=src
OBJ=obj
OUT=tetris

all: tetris

clean:
	rm -f $(OBJ)/*.o $(OUT)

.PHONE: all clean

$(OUT): $(SRC)/tetris.c $(OBJ)/utils.o $(OBJ)/display.o $(OBJ)/keyboard.o $(OBJ)/field.o $(OBJ)/player.o $(OBJ)/score.o $(OBJ)/audio.o $(OBJ)/randomiser.o $(OBJ)/mino.o $(OBJ)/hud.o $(OBJ)/game.o $(OBJ)/menu.o
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

$(OBJ)/utils.o: $(SRC)/utils.c $(SRC)/utils.h
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ)/display.o: $(SRC)/display.c $(SRC)/display.h
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ)/keyboard.o: $(SRC)/keyboard.c $(SRC)/keyboard.h
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ)/field.o: $(SRC)/field.c $(SRC)/field.h
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ)/player.o: $(SRC)/player.c $(SRC)/player.h
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ)/score.o: $(SRC)/score.c $(SRC)/score.h
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ)/audio.o: $(SRC)/audio.c $(SRC)/audio.h
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ)/randomiser.o: $(SRC)/randomiser_7bag.c $(SRC)/randomiser.h
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ)/mino.o: $(SRC)/mino.c $(SRC)/mino.h
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ)/hud.o: $(SRC)/hud.c $(SRC)/hud.h
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ)/game.o: $(SRC)/game.c $(SRC)/game.h
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ)/menu.o: $(SRC)/menu.c $(SRC)/menu.h
	$(CC) -c -o $@ $< $(CFLAGS)
