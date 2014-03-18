PROJECT_NAME = engine
FLAGS = -O0 -g -Wall -Wextra -Werror -Wunreachable-code -I ./
CC = gcc $(FLAGS)
LIBS = -lm $(shell sdl-config --libs) -lncurses
SHELL_TERMINAL = gnome-terminal --title="Graphics Engine: Shell" \
	--geometry=108x49+1000 --profile=Default -e

SRC = $(wildcard src/*.c src/**/*.c)
OBJ = $(patsubst %.c, bin/%.o, $(foreach srcFile, $(SRC), $(notdir $(srcFile))))

.PHONY: all run kill clean

all: bin bin/$(PROJECT_NAME)

run: all
	@$(SHELL_TERMINAL) bin/$(PROJECT_NAME) $(SCRIPT_FILE)

kill:
	@-killall -9 $(PROJECT_NAME)

clean:
	@rm -rf bin

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
