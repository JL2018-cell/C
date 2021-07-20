CXXFLAGS := -g -Wall -std=c++0x -lm
#CXXFLAGS := -g -Wall -lm
CXX=g++

all: processor

processor: processor.o driver.o
	$(CXX) -o processor processor.o driver.o

clean:
	rm -f processor *.o
