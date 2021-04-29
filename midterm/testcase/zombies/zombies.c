#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tools.c"

int is_zombie(int pid) {
        int fd = -1, rlen;
        char *tok, buf[1024];

        if(snprintf(buf, sizeof(buf), "/proc/%u/stat", pid) < 1) goto err;
        if((fd = open(buf, O_RDONLY)) < 0) { perror("open"); goto err; }
        if((rlen = read(fd, buf, sizeof(buf))) < 0) { perror("read"); goto err; }
        close(fd);
        fd = -1;

        if((tok = strtok(buf, " \t\n\r")) == NULL) goto err;
        if((tok = strtok(NULL, " \t\n\r")) == NULL) goto err;
        if((tok = strtok(NULL, " \t\n\r")) == NULL) goto err;

        return tok[0] == 'Z' ? 1 : 0;

err:
        if(fd >= 0) close(fd);
        return 0;
}
int compint(const void *a, const void *b) { return *((int*) a) - *((int*) b); }

int main(int argc, char *argv[]) {
        int pid, len, fd[2];
        int i, n, pids[128] = { 0 };
        char buf[8192];

        CHECKARG(argc, 2, "usage: zombies /path/to/your/file.so\n");

        unbuffered();
        signal(SIGALRM, _exit);
        srand(time(0) ^ getpid());

        n = 11 + rand() % 40;

        if(pipe(fd) < 0) { perror("pipe"); exit(-1); }

        if((pid = fork()) < 0) { perror("fork"); exit(-1); }

        if(pid == 0) {
                void *h;
                int (*zombies)(int n, int *pids);
                int sysexec[] = { SCMP_SYS(execve), SCMP_SYS(execveat) };

                dup2(fd[1], 1);
                close(fd[0]);
                close(fd[1]);

                reject_syscall(sysexec, sizeof(sysexec)/sizeof(int));
                LOAD_SO(h, argv[1]);
                LOAD_SYM(zombies, h, "zombies");

                if(zombies(n, pids) == n) {
                        int wlen = 0;
                        for(i = 0; i < n; i++) {
                                wlen += snprintf(buf+wlen, sizeof(buf)-wlen, "%d ", pids[i]);
                        }
                        write(1, buf, wlen);
                        zombies(0, NULL);
                }
        } else {
      alarm(3);
                printf("* waiting for zombies ... ");

                if((len = read(fd[0], buf, sizeof(buf)-1)) > 0) {
                        char *ptr = buf;
                        buf[len] = '\0';
                        printf("%s", buf);
                        for(i = 0; i < n; i++) {
                                char *tok;
                                if((tok = strtok(ptr, " \t\n\r")) == NULL) goto err;
                                pids[i] = strtoll(tok, NULL, 0);
                                ptr = NULL;
                        }
                        qsort(pids, n, sizeof(int), compint);
                        for(i = 0; i < n-1; i++) { if(pids[i] == pids[i+1]) goto err; }
                        printf("\n* checking for zombies ...");
                        for(i = 0; i < n; i++) {
                                if(is_zombie(pids[i]) == 0) goto err;
                                printf(" %d", pids[i]);
                        }
                        printf("\n");
                        readflag(0);
                }
        }

        return 0;

err:
        printf("No no no ...\n");
        return -1;

}

