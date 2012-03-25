CXX=g++
#CXX=g++-4.7
#CXX=clang++

FLAGS=-std=c++0x
# For gcc >= 4.7
#FLAGS=-std=gnu++11



all : program library
	$(CXX) $(FLAGS) -o randomclient *.o

program:
	$(CXX) $(FLAGS) -c src/*.cpp
library:
	$(CXX) $(FLAGS) -c lib/*.cpp

clean:
	rm *.o
