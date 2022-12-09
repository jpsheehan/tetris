CFLAGS=-Werror -Wall `pkg-config allegro-5 allegro_primitives-5 --cflags`
LFLAGS=`pkg-config allegro-5 allegro_font-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 allegro_image-5 --libs`
CC=gcc

all: tetris

clean:
	rm -f *.o tetris

.PHONE: all clean

tetris: tetris.c utils.o display.o keyboard.o
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

utils.o: utils.c utils.h
	$(CC) -c -o $@ $< $(CFLAGS)

display.o: display.c display.h
	$(CC) -c -o $@ $< $(CFLAGS)

keyboard.o: keyboard.c keyboard.h
	$(CC) -c -o $@ $< $(CFLAGS)
