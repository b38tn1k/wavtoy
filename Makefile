CC = g++
CFLAGS = -Wall -g 
TARGET = wavtoy 
OBJFILES = main.o wav_handler.o timeline.o synth.o score_handler.o
OBJS = include/frequencies.h wav_handler.o timeline.o synth.o score_handler.o

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES)

main.o: main.cpp  $(OBJS)
	$(CC) $(CFLAGS) -c main.cpp

wav_handler.o: include/wav_handler.h include/wav_handler.cpp
	$(CC) $(CFLAGS) -c include/wav_handler.cpp

timeline.o: include/timeline.h include/timeline.cpp
	$(CC) $(CFLAGS) -c include/timeline.cpp

synth.o: include/synth.h include/synth.cpp
	$(CC) $(CFLAGS) -c include/synth.cpp

score_handler.o: include/score_handler.h include/score_handler.cpp
	$(CC) $(CFLAGS) -c include/score_handler.cpp

clean:
	rm -rf *.o