CC = g++
CFLAGS = -Wall -g

soundtoy: main.o wav_handler.o timeline.o add_synth.o score_handler.o
	$(CC) $(CFLAGS) -o soundtoy main.o wav_handler.o timeline.o add_synth.o score_handler.o

main.o: main.cpp frequencies.h wav_handler.h timeline.h add_synth.h score_handler.h
	$(CC) $(CFLAGS) -c main.cpp

wav_handler.o: wav_handler.h

timeline.o: timeline.h

add_synth.o: add_synth.h

score_handler.o: score_handler.h

clean:
	rm -rf *.o