//#define debug
#ifdef debug
#include <stdio.h>
#include<sys/types.h>
#include<unistd.h>
#else 
#include "libmini.h"
#endif

int main() {
	sigset_t s;
	sigemptyset(&s);
	sigaddset(&s, SIGALRM);
	sigprocmask(SIG_BLOCK, &s, NULL);
	alarm(3);
	sleep(5);
	if(sigpending(&s) < 0) perror("sigpending");
	if(sigismember(&s, SIGALRM)) {
		char m[] = "sigalrm is pending.\n";
		write(1, m, sizeof(m));
	} else {
		char m[] = "sigalrm is not pending.\n";
		write(1, m, sizeof(m));
	}
	return 0;
}
