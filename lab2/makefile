
all:logger.so logger

logger:hw2.cpp comm.h
	g++ $< -o $@ -g 
	
logger.so:Logger.cpp hook.cpp marco.h comm.h Logger.h
	g++ $^ -o $@ -shared -fPIC -g -ldl

test:test.c
	gcc $< -o $@ -g 

testcase:
	LD_PRELOAD=./logger.so ./test
clean:
	rm logger.so logger