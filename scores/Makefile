CC = g++
CFLAGS = -Wall -g -lncurses
TARGET = wavtoy-composer 
OBJFILES = main.o metahandler.o

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES)

main.o: main.cpp  $(OBJS)
	$(CC) $(CFLAGS) -c main.cpp

metahandler.o: include/metahandler.h include/metahandler.cpp
	$(CC) $(CFLAGS) -c include/metahandler.cpp

clean:
	rm -rf *.o