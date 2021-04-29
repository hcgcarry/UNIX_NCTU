#include "tools.c"

#define MAXN    8888

int main() {
        int i, last = -1;

        unbuffered();

        srand(time(0) ^ getpid());

        printf("Guess the number for %d times!\nFor each guess, please enter five digits and press enter: ", MAXN);

        for(i = 0; i < MAXN; i++) {
                int g, n, rlen;
                char buf[16];
                n = 1 + (rand() & 0xffff);
                if(last == n) {
                        fprintf(stderr, "bad random number generator?\n");
                        exit(-1);
                }
                last = n;
                if((rlen = read(0, buf, 6)) != 6) {
                        fprintf(stderr, "read error\n");
                        exit(-1);
                }
                g = strtol(buf, NULL, 0);
                if(g != n) {
                        fprintf(stderr, "%d != %d\n", g, n);
                        exit(-1);
                }
                printf(".");
        }

       if(i == MAXN) {
                printf("Bingo!\n");
                readflag(0);
        } else {
                printf("Nope!\n");
        }

        return 0;
}


