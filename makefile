CC = gcc

all: bin bin/engine

run: all
	bin/engine

clean:
	if [ -d "bin" ]; then rm -rf bin; fi

bin/engine: src/engine.c
	$(CC) -o $@ $^

bin:
	mkdir bin
