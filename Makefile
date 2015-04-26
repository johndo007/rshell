all: rshell
	mkdir -p ./bin
rshell: 
	g++ -std=c++11 -Wall -Werror -pedantic -ansi -pedantic ./src/main.cpp ./src/shell.h ./src/ls_cmd.h -o ./bin/rshell
clean:
	rm -rf ./bin

