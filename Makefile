CC = g++
CFLAGS1 = -Wall -g 
TARGET1 = wavtoy
TARGET2 = wavtoy-composer 
OBJFILES1 = wavtoy-main.o wav_handler.o timeline.o synth.o score_handler.o
OBJS1 = include/frequencies.h wav_handler.o timeline.o synth.o score_handler.o
CFLAGS2 = -Wall -g -lncurses
TARGET2 = wavtoy-composer 
OBJFILES2 = wavtoy-composer-main.o metahandler.o cursestable.o
OBJS2 = metahandler.o cursestable.o

$(TARGET1): $(OBJFILES1)
	$(CC) $(CFLAGS1) -o $(TARGET1) $(OBJFILES1)

$(TARGET2): $(OBJFILES2)
	$(CC) $(CFLAGS2) -o $(TARGET2) $(OBJFILES2)

wavtoy-composer-main.o: wavtoy-composer-main.cpp  $(OBJS2)
	$(CC) $(CFLAGS1) -c wavtoy-composer-main.cpp $(OBJS2)

metahandler.o: include/metahandler.h include/metahandler.cpp cursestable.o
	$(CC) $(CFLAGS1) -c include/metahandler.cpp cursestable.o

cursestable.o: include/cursestable.h include/cursestable.cpp
	$(CC) $(CFLAGS1) -c include/cursestable.cpp

wavtoy-main.o: wavtoy-main.cpp  $(OBJS1)
	$(CC) $(CFLAGS1) -c wavtoy-main.cpp

wav_handler.o: include/wav_handler.h include/wav_handler.cpp
	$(CC) $(CFLAGS1) -c include/wav_handler.cpp

timeline.o: include/timeline.h include/timeline.cpp
	$(CC) $(CFLAGS1) -c include/timeline.cpp

synth.o: include/synth.h include/synth.cpp
	$(CC) $(CFLAGS1) -c include/synth.cpp

score_handler.o: include/score_handler.h include/score_handler.cpp
	$(CC) $(CFLAGS1) -c include/score_handler.cpp



clean:
	rm -rf *.o

all: $(TARGET1) $(TARGET2)

target1: $(TARGET1)

target2: $(TARGET2)