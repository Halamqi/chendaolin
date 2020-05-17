src=$(wildcard *.c)

ALL:main

myArg=-Wall -pthread -g

main:$(src)
	gcc -o $@ $^ $(myArg)
clear:
	-rm -rf main
.PHONY:ALL clear
