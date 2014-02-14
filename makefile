FLAGS = -Ofast -Wall -Wextra -I .
CC = gcc $(FLAGS)
LIBS = -lSDL -lm

all: bin bin/engine

run: all
	bin/engine

clean:
	if [ -d "bin" ]; then rm -rf bin; fi

bin/engine: bin/engine.o bin/screen.o
	$(CC) -o $@ $^ $(LIBS)

bin/engine.o: src/engine.c
	$(CC) -o $@ -c $^

bin/screen.o: src/screen.c
	$(CC) -o $@ -c $^

bin:
	mkdir bin
