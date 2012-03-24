CXX=g++

all : program library
	$(CXX) -o randomclient *.o

program:
	$(CXX) -c src/*.cpp
library:
	$(CXX) -c lib/*.cpp

clean:
	rm *.o
