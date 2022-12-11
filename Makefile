CFLAGS=-g -Werror -Wall `pkg-config allegro-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5  --cflags`
LFLAGS=`pkg-config allegro-5 allegro_font-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 --libs`
CC=gcc

all: tetris

clean:
	rm -f *.o tetris

.PHONE: all clean

tetris: tetris.c utils.o display.o keyboard.o field.o player.o score.o audio.o randomiser.o
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

utils.o: utils.c utils.h
	$(CC) -c -o $@ $< $(CFLAGS)

display.o: display.c display.h
	$(CC) -c -o $@ $< $(CFLAGS)

keyboard.o: keyboard.c keyboard.h
	$(CC) -c -o $@ $< $(CFLAGS)

field.o: field.c field.h
	$(CC) -c -o $@ $< $(CFLAGS)

player.o: player.c player.h
	$(CC) -c -o $@ $< $(CFLAGS)

score.o: score.c score.h
	$(CC) -c -o $@ $< $(CFLAGS)

audio.o: audio.c audio.h
	$(CC) -c -o $@ $< $(CFLAGS)

randomiser.o: randomiser_7bag.c randomiser.h
	$(CC) -c -o $@ $< $(CFLAGS)
