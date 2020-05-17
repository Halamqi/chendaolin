src=$(wildcard *.c)

obj=$(patsubst %.c,%,$(src))

ALL:$(obj)

myArgs=-Wall -pthread -g

%:%.c
	gcc $< -o $@ $(myArgs) 

clean :
	-rm -rf $(obj) 
.PHONY:clean ALL
