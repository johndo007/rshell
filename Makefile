all: rshell
	mkdir -p ./bin
rshell: 
	mkdir -p ./bin  
	g++ -std=c++11 -Wall -Werror -pedantic -ansi -pedantic ./src/main.cpp ./src/shell.h ./src/ls_cmd.h ./src/redirect.h -o ./bin/rshell 
cp:
	g++ -std=c++11 -Wall -Werror -pedantic -ansi -pedantic ./src/cp.cpp -o ./bin/cp.cpp
clean:
	rm -rf ./bin

