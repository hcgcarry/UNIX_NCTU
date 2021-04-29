#include <stdio.h>
#include <string.h>
#include "tools.c"

static char msg[] = "hello, world!";

int main(int argc, char *argv[]) {
        char buf[256];
        int pid, len, fd[2];

        CHECKARG(argc, 2, "usage: helloworld /path/to/your/file.so\n");

        signal(SIGALRM, _exit);

        if(pipe(fd) < 0) { perror("pipe"); exit(-1); }

        if((pid = fork()) < 0) { perror("fork"); exit(-1); }

        if(pid == 0) {
                void *h;
                int (*helloworld)(char *buf, int bufsz);
                int sysexec[] = { SCMP_SYS(execve), SCMP_SYS(execveat) };
                //int sysopen[] = { SCMP_SYS(open), SCMP_SYS(openat), SCMP_SYS(open_by_handle_at) };

                dup2(fd[1], 1);
                close(fd[0]);
                close(fd[1]);

                reject_syscall(sysexec, sizeof(sysexec)/sizeof(int));
                LOAD_SO(h, argv[1]);
                LOAD_SYM(helloworld, h, "helloworld");

                if((len = helloworld(buf, sizeof(buf))) > 0) {
                        write(1, buf, len);
                }

                exit(0);
        } else {
                          alarm(1);
                if((len = read(fd[0], buf, sizeof(buf)-1)) > 0) {
                        buf[len] = '\0';
                        if(strcmp(msg, buf) != 0) {
                                printf("No no no ...\n");
                        } else {
                                printf("Bingo!\n");
                                readflag(0);
                        }
                } else {
                        perror("read");
                }
        }

        return 0;
}


