all: rshell
rshell:
	mkdir -p ./bin
	g++ -Wall -Werror -pedantic -ansi -pedantic ./src/main.cpp shell.h -o ./bin/rshell
clean:
	rm -rf ./bin

