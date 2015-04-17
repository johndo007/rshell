bin:
  	mkdir bin
rshell: bin
  	g++ -Wall -Werror -std=c++11 -pedantic ./src/hw0.cpp -o ./bin/rshell
