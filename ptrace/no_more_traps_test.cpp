#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <stdlib.h>     /* strtol */

#include<fcntl.h>
#include <sys/user.h>
#include<fstream>

unsigned long getCode();

using namespace std;

void errquit(const char *msg) {
	perror(msg);
	exit(-1);
}

void dump_code(long addr, long code) {
	fprintf(stderr, "## %lx: code = %02x %02x %02x %02x %02x %02x %02x %02x\n",
		addr,
		((unsigned char *) (&code))[0],
		((unsigned char *) (&code))[1],
		((unsigned char *) (&code))[2],
		((unsigned char *) (&code))[3],
		((unsigned char *) (&code))[4],
		((unsigned char *) (&code))[5],
		((unsigned char *) (&code))[6],
		((unsigned char *) (&code))[7]);
}

int main(int argc, char *argv[]) {
	pid_t child;
	if((child = fork()) < 0) errquit("fork");
	if(child == 0) {
		if(ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) errquit("ptrace");
		execlp("./no_more_traps", "./no_more_traps", NULL);
		errquit("execvp");
	} else {
		int status;

		if(waitpid(child, &status, 0) < 0) errquit("waitpid");
		assert(WIFSTOPPED(status));
		ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL);
		/* set break point */
		/* continue the execution */
		ptrace(PTRACE_CONT, child, 0, 0);

		while(waitpid(child, &status, 0) > 0) {
			struct user_regs_struct regs;
			if(!WIFSTOPPED(status)) continue;
			if(ptrace(PTRACE_GETREGS, child, 0, &regs) != 0){
				errquit("ptrace(GETREGS)");
			}

			unsigned long code = ptrace(PTRACE_PEEKTEXT, child, regs.rip-1,0) ;
			unsigned long newCode=(code & 0xffffffffffffff00 | getCode());
			if(ptrace(PTRACE_POKETEXT, child, regs.rip-1, newCode) != 0)
				errquit("ptrace(POKETEXT)");
			/* set registers */
			regs.rip = regs.rip-1;
			if(ptrace(PTRACE_SETREGS, child, 0, &regs) != 0) errquit("ptrace(SETREGS)");
			ptrace(PTRACE_CONT, child, 0, 0);
		}
	}
	return 0;
}


unsigned long getCode(){
	static FILE* inputFIle = fopen("no_more_traps.txt","r");
	char buf[100];
	fgets(buf,3,inputFIle);
	unsigned long hex = strtol(buf,NULL,16);
	printf("get code:%s\n",buf);
	printf("get code num version:%lx\n",hex);
	return hex;

}

