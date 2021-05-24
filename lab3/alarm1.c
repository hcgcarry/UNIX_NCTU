
//#define debug
#ifdef debug
#include <stdio.h>
#include<sys/types.h>
#include<unistd.h>
#else 
#include "libmini.h"
#endif
int main() {
	alarm(3);
	pause();
	return 0;
}