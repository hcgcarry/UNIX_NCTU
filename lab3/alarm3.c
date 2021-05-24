
//#define debug
#ifdef debug
#include <stdio.h>
#include<sys/types.h>
#include<unistd.h>
#else 
#include "libmini.h"
#endif

void handler(int s) { /* do nothing */ }
void alarmhandler(int s) { 
		char m[] = "alarm.\n";
		write(1, m, sizeof(m));
}

int main() {
	sigset_t s;
	sigemptyset(&s);
	sigaddset(&s, SIGALRM);
	sigprocmask(SIG_BLOCK, &s, NULL);
	//signal(SIGALRM, SIG_IGN);
	if(signal(SIGALRM, alarmhandler)<0){
		char m[] = "sigalrm signal failed.\n";
		write(1, m, sizeof(m));
	}
	if(signal(SIGINT, handler)<0){
		char m[] = "sigint signal failed.\n";
		write(1, m, sizeof(m));
	}
	alarm(1);
	pause();
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
