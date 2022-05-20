CC=gcc -Wall

PROGS=barbeiro

all: $(PROGS)

clean:
	rm -f $(PROGS)

barbeiro: main.c barbeiro_monitor.c barbeiro_monitor.h
	$(CC) main.c barbeiro_monitor.c -o barbeiro -lpthread
