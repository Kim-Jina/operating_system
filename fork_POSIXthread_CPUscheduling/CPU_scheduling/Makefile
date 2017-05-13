OBJS1 = filegen.c
OBJS2 = schedtest.c
EXEC1 = filegen
EXEC2 = schedtest

CFLAGS = -g

CC = gcc

all: $(OBJS1) $(OBJS2)
	$(CC) -o $(EXEC1) $(OBJS1)
	$(CC) -o $(EXEC2) $(OBJS2) -lrt

clean:
	rm -rf $(EXEC1)
	rm -rf $(EXEC2)
