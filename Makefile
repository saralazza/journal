CC := gcc

CFLAGS:= -Wall

all: monoprocesso multiprocesso

hw_monoprocesso.o: hw_monoprocesso.c
	$(CC) $(CFLAG) -c $^

hw_multiprocesso.o: hw_multiprocesso.c
	$(CC) $(CFLAGS) -c $^

utils.o: utils.c
	$(CC) $(CFLAGS) -c $^

monoprocesso: hw_monoprocesso.o utils.o
	$(CC) $(CFLAGS) $^ -o monoprocesso

multiprocesso: hw_multiprocesso.o utils.o
	$(CC) $(CFLAGS) $^ -o multiprocesso

clean:
	@rm *.o monoprocesso multiprocesso

.PHONY: clean

