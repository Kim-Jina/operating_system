OBJS1 = fork.c
OBJS2= thread.c
EXEC1 = fork
EXEC2 = thread

CFLAGS = -g

CC = gcc

all: $(OBJS1) $(OBJS2)
	$(CC) -o $(EXEC1) $(OBJS1) -lrt
	$(CC) $(OBJS2) -o $(EXEC2) -lpthread -lrt

clean:
	rm -rf $(EXEC1)
	rm -rf $(EXEC2)
