all: bin rshell cp
bin:
  mkdir bin
rshell:
  g++ -Wall -Werror -pedantic -ansi src/hw0.cpp -o ./bin/rshell
cp:
  g++ -Wall -Werror -ansi -pedantic src/cp.cpp -o ./bin/cp
