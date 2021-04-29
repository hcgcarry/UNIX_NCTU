#include <string.h>
#include "tools.c"

int main(int argc, char *argv[]) {
        unbuffered();
        if(strstr(argv[0], "UnixProgramming")) {
                printf("Bingo!\n");
                readflag(1);
        } else {
                printf("Nope!\n");
        }
        return 0;
}
