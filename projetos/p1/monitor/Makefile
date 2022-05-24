CC=gcc -Wall

PROGS=monitor

all: $(PROGS)

clean:
	rm -f $(PROGS)

monitor: main.c monitor.c monitor.h
	$(CC) main.c monitor.c -o monitor -lpthread
