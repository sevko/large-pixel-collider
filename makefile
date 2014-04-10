SCRIPT_FILE = 
PROJECT_NAME = engine
FLAGS = -Ofast -Wall -Wextra -Werror -Wunreachable-code -I ./
CC = gcc $(FLAGS)
LIBS = -lm $(shell sdl-config --libs) -lncurses
SHELL_TERMINAL = gnome-terminal --title="Graphics Engine: Shell" \
	--geometry=78x49+1000 --profile=Default -e

SRC = $(wildcard src/*.c src/**/*.c)
OBJ = $(patsubst %.c, bin/%.o, $(foreach srcFile, $(SRC), $(notdir $(srcFile))))

.PHONY: all run test kill clean install

all: bin bin/$(PROJECT_NAME)

bin:
	@mkdir $@

bin/$(PROJECT_NAME): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

bin/%.o: src/%.c
	$(CC) -o $@ -c $^

bin/screen.o: src/screen.c
	$(CC) -o $@ -c $^ $(shell sdl-config --cflags)

bin/%.o: src/interpreter/%.c
	$(CC) -o $@ -c $^

run: all
	@if [ "$(SCRIPT_FILE)" != "" ]; then \
		bin/$(PROJECT_NAME) --script $(SCRIPT_FILE); \
	else \
		$(SHELL_TERMINAL) bin/$(PROJECT_NAME); \
	fi

test: all
	@bin/engine --test

kill:
	@-killall -9 $(PROJECT_NAME)

clean:
	@rm -rf bin

install:
	@sh install.sh
