#include "tools.c"

int main() {
        int g, n, rlen;
        char buf[16] = "";

        unbuffered();

        srand(time(0) ^ getpid());

        printf("Guess the number!\nPlease enter five digits and press enter: ");

        n = 1 + (rand() & 0xffff);

        if((rlen = read(0, buf, 6)) <= 0) {
                fprintf(stderr, "read error\n");
                exit(-1);
        }

        g = strtol(buf, NULL, 0);
        if(g == n) {
                printf("Bingo!\n");
                readflag(0);
        } else {
                printf("Nope!\n");
        }

        return 0;
}

