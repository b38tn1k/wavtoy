CC = g++
CFLAGS = -Wall -g

soundtoy: main.o wav_handler.o timeline.o synth.o score_handler.o
	$(CC) $(CFLAGS) -o soundtoy main.o wav_handler.o timeline.o synth.o score_handler.o

main.o: main.cpp frequencies.h wav_handler.h timeline.h synth.h score_handler.h
	$(CC) $(CFLAGS) -c main.cpp

wav_handler.o: wav_handler.h

timeline.o: timeline.h

synth.o: synth.h

score_handler.o: score_handler.h

clean:
	rm -rf *.o