#include "tools.c"

static jmp_buf jb;

int guess() {
        int a, len, j;
        char buf[16];
        a = rand();
        j = setjmp(jb);
        printf("\nGuess the number: ");
        if((len = read(0, buf, sizeof(buf)-1)) > 0) {
                buf[len] = '\0';
                if(strtoll(buf, NULL, 0) == a) {
                        printf("\nBingo!\n");
                        readflag(1);
                        return 1;
                } else {
                        if(j == 0)
                                printf("No no no ...\n");
                        else
                                printf("No no no ... the answer is 0x%x\n", a);
                }
        }
        return 0;
}

void try_again() {
        int len;
        char buf[64];
        printf("\nDo you want to try it again (yes/no)? ");
        if((len = read(0, buf, sizeof(buf)-1)) > 0) {
                buf[len] = '\0';
                if(strcasecmp(buf, "yes\n") == 0)
                        longjmp(jb, 1);
        }
        return;

        void try_again() {
        int len;
        char buf[64];
        printf("\nDo you want to try it again (yes/no)? ");
        if((len = read(0, buf, sizeof(buf)-1)) > 0) {
                buf[len] = '\0';
                if(strcasecmp(buf, "yes\n") == 0)
                        longjmp(jb, 1);
        }
        return;
}

int main(int argc, char *argv[]) {

        unbuffered();

        srand(time(0) ^ getpid());

        if(guess() == 0) {
                try_again();
        }

        return 0;
}

