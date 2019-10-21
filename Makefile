all: bin/dhackasm

bin:
	mkdir bin

bin/dhackasm: $(wildcard src/*.c) bin
	gcc src/*.c -o bin/dhackasm