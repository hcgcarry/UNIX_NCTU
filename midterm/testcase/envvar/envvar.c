/*
 * Exam problem set for UNIX programming course
 * by Chun-Ying Huang <chuang@cs.nctu.edu.tw>
 * License: GPLv2
 */
#include <sys/prctl.h>
#include "tools.c"

char secret[128];
static pid_t child = 0;

#define CHECKENV        "checkenv"
#define xerror(x)       { perror(x); exit(-1); }

void killchild()        { if(child > 0) kill(child, SIGKILL); }
void sigterm(int s)     { killchild(); exit(-1); }

int check_envvar(char *secret) {
        char buf[1024];
        int s, len = 0;

        snprintf(buf, sizeof(buf), "/proc/%d/environ", child);
        if((s = open(buf, O_RDONLY)) < 0) { perror("open"); return -1; }
        if((len = read(s, buf, sizeof(buf))) <= 0) perror("read");
        close(s);
        if(len <= 0) return -1;

        for(s = 0; s < len; s = s + strlen(&buf[s]) + 1) {
                if(strncmp(&buf[s], "SECRET=", 7) == 0) {
                        if(strcmp(secret, &buf[s+7]) == 0) return 0;
                        break;
                }
        }

        return -1;
}
int main(int argc, char *argv[]) {
        char buf[1024];
        int fdin[2], fdout[2];
        void *h;
        void (*f)(int shortcut, char *secret);

        CHECKARG(argc, 2, "usage: envvar /path/to/your/file.so\n");
        LOAD_SO(h, argv[1]);
        LOAD_SYM(f, h, "envvar");

        if(getenv("SHORTCUT") != NULL) {
                f(1, NULL);
                exit(0);
        }

        atexit(killchild);
        setvbuf(stdout, NULL, _IONBF, 0);
        setvbuf(stderr, NULL, _IONBF, 0);
        signal(SIGCHLD, SIG_IGN);
        signal(SIGTERM, sigterm);
        signal(SIGINT, SIG_IGN);

        do { // create a random secret
                unsigned long long i, x = 0ULL;
                srand(time(0) ^ getpid());
                for(i = 0; i < 4; i++) { x <<= 16; x |= rand() ^ 0xffff; }
                snprintf(secret, sizeof(secret), "%llu%llu", x, x>>1);
        } while(0);

        if(pipe(fdin) < 0)              xerror("pipe");
        if(pipe(fdout) < 0)             xerror("pipe");
        if((child = fork()) < 0)        xerror("fork");
        if(child == 0) {
                if(prctl(PR_SET_DUMPABLE, 1/*SUID_DUMP_USER*/, 0, 0, 0) < 0) perror("prctl");
                signal(SIGCHLD, SIG_DFL);
                dup2(fdin[1], 1);  close(fdin[0]);  close(fdin[1]);
                dup2(fdout[0], 0); close(fdout[0]); close(fdout[1]);
                alarm(10);
                      f(0, secret);
                write(1, CHECKENV, strlen(CHECKENV));
                read(0, buf, sizeof(buf));
        } else {
                int s;
                close(fdin[1]);
                close(fdout[0]);
                do if((s = read(fdin[0], buf, sizeof(buf))) > 0) {
                        if(strncmp(buf, CHECKENV, s) != 0) break;
                        if(check_envvar(secret) < 0) break;
                        printf("Bingo!\n");
                        readflag(0);
                        write(fdout[1], "pass", 4);
                        exit(0);
                } while(0);
                write(fdout[1], "fail", 4);
                printf("No no no ...\n");
        }
        return -1;
}



