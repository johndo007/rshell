all: bin rshell
bin:
  mkdir bin
rshell:
  g++ -Wall -Werror -pedantic -ansi src/hw0.cpp -o ./bin/rshell
