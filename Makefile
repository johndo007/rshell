bin:
	    mkdir bin

rshell: bin
		g++  ./src/hw0.cpp -o ./bin/rshell
run: rshell
		(cd bin && ./rshell)
clean:
	    rm -rf bin
edit: 
		(cd src && vim hw0.cpp)
