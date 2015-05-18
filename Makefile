all: rshell
	mkdir -p ./bin
rshell: 
	mkdir -p ./bin  
	g++ -std=c++11 -Wall -Werror -pedantic -ansi -pedantic -c ./src/main.cpp -o ./bin/main.o 
	g++ -std=c++11 -Wall -Werror -pedantic -ansi -pedantic -c ./src/shell.cpp -o ./bin/shell.o
	g++ -std=c++11 -Wall -Werror -pedantic -ansi -pedantic -c ./src/ls_cmd.cpp -o ./bin/ls_cmd.o
	g++ -std=c++11 -Wall -Werror -pedantic -ansi -pedantic -c ./src/redirect.cpp -o ./bin/redirect.o
	g++ -std=c++11 -Wall -Werror -pedantic -ansi -pedantic -o ./bin/rshell ./bin/shell.o ./bin/ls_cmd.o ./bin/redirect.o ./bin/main.o
cp:
	g++ -std=c++11 -Wall -Werror -pedantic -ansi -pedantic ./src/lab/cp.cpp -o ./bin/cp.cpp
clean:
	rm -rf ./bin

