
all:logger.so logger

logger:hw2.cpp comm.h
	g++ $< -o $@ -g 
	
logger.so:Logger.cpp hook.cpp marco.h comm.h Logger.h
	g++ $^ -o $@ -shared -fPIC -g -ldl

%64:%.c
	gcc $< -o $@ -g -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
%:%.c
	gcc $< -o $@ -g 

testcase:
	LD_PRELOAD=./logger.so ./test
clean:
	rm logger.so logger