CC=gcc -Wall

PROGS=readFifo writeFifo

all: $(PROGS)

clean:
	rm -f $(PROGS)

read_fifo: readFifo.c
	$(CC) $< -o $@

write_fifo: writeFifo.c
	$(CC) $< -o $@