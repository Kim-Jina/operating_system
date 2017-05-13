OBJS = bbp.c
EXEC = bbp

CFLAGS = -g

CC = gcc

all: $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) -lpthread -DMUTEX
	
clean:
	rm -rf $(EXEC)
