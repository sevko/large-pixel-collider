FLAGS = -O0 -g -Wall -Wextra -Werror -Wunreachable-code -I ./
CC = gcc $(FLAGS)
LIBS = -lm $(shell sdl-config --libs) -lncurses

SHELL_TERMINAL = gnome-terminal --title="Graphics Engine" \
	--geometry=108x49+1000 -e

all: bin bin/engine

run: all
	bin/engine $(SCRIPT_FILE)

kill:
	killall -9 engine

clean:
	if [ -d "bin" ]; then rm -rf bin; fi

bin/engine: bin/engine.o bin/utils.o bin/matrix.o bin/screen.o \
	bin/interpreter.o bin/file_parser.o bin/shell.o bin/shell_graphics.o
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

bin/shell.o: src/interpreter/shell.c
	$(CC) -o $@ -c $^

bin/shell_graphics.o: src/interpreter/shell_graphics.c
	$(CC) -o $@ -c $^

bin:
	mkdir bin
