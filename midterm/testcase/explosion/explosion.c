#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "tools.c"

#define EXPLOSION       "/tmpfs/explosion"
#define TARGET          (0x01<<30)

int main() {
        struct stat st;

        unbuffered();

        printf("* checking %s ...\n", EXPLOSION);

        if(lstat(EXPLOSION, &st) < 0) {
                perror("stat");
                return -1;
        }

        if(st.st_size == TARGET) {
                printf("Bingo!\n");
                readflag(1);
        } else {
                printf("! unexpected size %lu (!=%u) \n", st.st_size, TARGET);
                printf("Nope!\n");
        }

        return 0;
}

