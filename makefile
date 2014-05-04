SCRIPT_FILE =
PROJECT_NAME = engine
FLAGS = -Ofast -Wall -Wextra -Wunreachable-code -I ./
CC = gcc $(FLAGS)
LIBS = -lm $(shell sdl-config --libs) -lncurses
SHELL_TERMINAL = gnome-terminal --title="Graphics Engine: Shell" \
	--geometry=78x49+1000 --profile=Default -e

SRC = $(shell find src -name "*.c" -printf "%p ")
OBJ = $(patsubst %.c, bin/%.o, $(foreach srcFile, $(SRC), $(notdir $(srcFile))))

.PHONY: all run test kill clean install

all: bin bin/$(PROJECT_NAME)

bin:
	@mkdir $@

bin/$(PROJECT_NAME): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

bin/%.o: src/%.c
	$(CC) -o $@ -c $^

bin/screen.o: src/graphics/screen.c
	$(CC) -o $@ -c $^ $(shell sdl-config --cflags)

bin/%.o: src/graphics/%.c
	$(CC) -o $@ -c $^

bin/%.o: src/interpreter/%.c
	$(CC) -o $@ -c $^

bin/%.o: src/interpreter/stack/%.c
	$(CC) -o $@ -c $^

run: all kill
	@if [ "$(SCRIPT_FILE)" != "" ]; then \
		bin/$(PROJECT_NAME) --script $(SCRIPT_FILE); \
	else \
		bin/$(PROJECT_NAME); \
	fi

test: all
	@bin/engine --test

kill:
	@if [ "$(shell pgrep $(PROJECT_NAME))" != "" ]; then \
		killall -9 $(PROJECT_NAME); \
	fi

clean:
	@rm -rf bin

install:
	@sh install.sh
