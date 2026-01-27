#Makefile for warmup1
#
# To Build: make warmup1
# To Clean: make clean

CC = gcc
CFLAGS = -Wall -g

TARGET = warmup1

OBJS = warmup1.o my402list.o

HDRS = my402list.h cs402.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

warmup1.o: warmup1.c $(HDRS)
	$(CC) $(CFLAGS) -c warmup1.c -o warmup1.o

my402list.o: my402list.c $(HDRS)
	$(CC) $(CFLAGS) -c my402list.c -o my402list.o

clean:
	rm -f $(OBJS) $(TARGET) *.gch core