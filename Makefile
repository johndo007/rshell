all: rshell
	mkdir -p ./bin
rshell: 
	mkdir -p ./bin  
	g++ -std=c++11 -Wall -Werror -pedantic -ansi -pedantic -c ./src/main.cpp 
	g++ -std=c++11 -Wall -Werror -pedantic -ansi -pedantic -c ./src/shell.cpp 
	g++ -std=c++11 -Wall -Werror -pedantic -ansi -pedantic -c ./src/ls_cmd.cpp
	g++ -std=c++11 -Wall -Werror -pedantic -ansi -pedantic -c ./src/redirect.cpp
	g++ -std=c++11 -Wall -Werror -pedantic -ansi -pedantic -o ./bin/rshell shell.o ls_cmd.o redirect.o main.o
cp:
	g++ -std=c++11 -Wall -Werror -pedantic -ansi -pedantic ./src/lab/cp.cpp -o ./bin/cp.cpp
clean:
	rm -rf ./bin

