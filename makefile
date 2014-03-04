FLAGS = -Ofast -Wall -Wextra -Wunreachable-code -I ./
CC = gcc $(FLAGS)
LIBS = -lm $(shell sdl-config --libs)

all: bin bin/engine

run: all
	bin/engine

clean:
	if [ -d "bin" ]; then rm -rf bin; fi

bin/engine: bin/engine.o bin/utils.o bin/matrix.o bin/screen.o bin/interpreter.o bin/file_parser.o
	$(CC) -o $@ $^ $(LIBS)

bin/engine.o: src/engine.c
	$(CC) -o $@ -c $^

bin/utils.o: src/utils.c
	$(CC) -o $@ -c $^

bin/matrix.o: src/matrix.c
	$(CC) -o $@ -c $^

bin/screen.o: src/screen.c
	$(CC) -o $@ -c $^

bin/interpreter.o: src/interpreter/interpreter.c
	$(CC) -o $@ -c $^

bin/file_parser.o: src/interpreter/file_parser.c
	$(CC) -o $@ -c $^

bin:
	mkdir bin
