SCRIPT_FILE =
PROJECT_NAME = engine
FLAGS = -Ofast -g -Wall -Wextra -Wunreachable-code -I ./
LIBS = -lm $(shell sdl-config --libs) -lncurses
C_COMPILER = gcc $(FLAGS)

CC = @echo "\tCC $@" && $(C_COMPILER)
SHELL_TERMINAL = gnome-terminal --title="Graphics Engine: Shell" \
	--geometry=78x49+1000 --profile=Default -e

PARSER_DEP = bin/y.tab.o bin/lex.yy.o
SRC = $(shell find src lib -name "*.c" -printf "%p ")
HEADERS = $(shell find src lib -name "*.h" -printf "%p ")
OBJ = $(patsubst %.c, bin/%.o, $(foreach srcFile, $(SRC), $(notdir $(srcFile))))

.PHONY: all run test kill clean install

all: bin bin/$(PROJECT_NAME)

bin:
	@mkdir $@

bin/$(PROJECT_NAME): $(PARSER_DEP) $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

bin/%.o: src/%.c
	$(CC) -o $@ -c $^

bin/%.o: src/graphics/%.c
	$(CC) -o $@ -c $^

bin/%.o: src/interpreter/%.c
	$(CC) -o $@ -c $^

bin/%.o: src/interpreter/stack/%.c
	$(CC) -o $@ -c $^

bin/lex.yy.c: lib/mdl.l
	@flex --outfile=$@ -I lib/mdl.l

bin/y.tab.c: lib/mdl.y
	@bison --output-file=$@ -d -y lib/mdl.y

bin/%.o: lib/%.c
	$(CC) -I bin -w -o $@ -c $^

bin/%.o: bin/%.c
	$(CC) -I bin -I lib -w -o $@ -c $^

bin/screen.o: src/graphics/screen.c
	$(CC) -o $@ -c $^ $(shell sdl-config --cflags)

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
