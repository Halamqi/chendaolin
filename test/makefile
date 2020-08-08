src=$(wildcard ../mybase/*.cpp)

all:test

test:$(src)
	g++ -o $@ $< -Wall -g -lpthread -I ../mybase/ -std=c++11 

clean:
	rm -rf $(obj)

.PHONY:clean all
