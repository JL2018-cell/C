# Author: Lin Jianan
# U. No.: 3035577595
# Filename: Makefile
# Assignment 2
# Description: Compile main.cpp, d2boh.cpp and d2boh.h to a executable which convert decimal number to binary, octal, and hexadecimal number.
# Last Changed: November 7, 2020

d2boh.o: d2boh.cpp d2boh.h
	g++ -pedantic-errors -std=c++11 -c d2boh.cpp

main.o: main.cpp d2boh.h
	g++ -pedantic-errors -std=c++11 -c main.cpp

main: d2boh.o main.o
	g++ -pedantic-errors -std=c++11 d2boh.o main.o -o main

clean:
	rm -f -r main d2boh *.o

.PHONY: clean

