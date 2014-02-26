FLAGS = -Ofast -Wall -Wextra -I .
CC = gcc $(FLAGS)
LIBS = -lm $(shell sdl-config --libs)

all: bin bin/engine

run: all
	bin/engine

clean:
	if [ -d "bin" ]; then rm -rf bin; fi

bin/engine: bin/engine.o bin/utils.o bin/screen.o bin/matrix.o
	$(CC) -o $@ $^ $(LIBS)

bin/engine.o: src/engine.c
	$(CC) -o $@ -c $^

bin/utils.o: src/utils.c
	$(CC) -o $@ -c $^

bin/matrix.o: src/matrix.c
	$(CC) -o $@ -c $^

bin/screen.o: src/screen.c
	$(CC) -o $@ -c $^

bin:
	mkdir bin
