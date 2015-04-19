all: rshell 
rshell:
	mkdir -p ./bin
	g++ -Wall -Werror -pedantic -ansi -pedantic ./src/hw0.cpp -o ./bin/rshell
clean:
	rm -rf ./bin

