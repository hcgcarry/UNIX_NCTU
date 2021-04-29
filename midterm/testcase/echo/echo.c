#include "tools.c"

#define N       1000

int readword(int n) {
        int i, len;
        char buf[256], word[16];
        len = 1 + rand() % 8;
        for(i = 0; i < len; i++) word[i] = 'a' + rand() % 26;
        word[i] = '\0';
        printf("\n%d. Please type '%s' (without quotes) and press enter: ", n, word);
        if((len = read(0, buf, sizeof(buf))) <= 0) return -1;
        if(buf[len-1] == '\n' && strncmp(word, buf, strlen(word)) == 0) {
                return 0;
        }
        return -1;
}

int main() {
        int i;
        srand(time(0) ^ getpid());
        unbuffered();
        printf( "Please follow my instructions shown below.\n"
                "Note that there are total %d instructions to follow. Good luck!\n", N);
        for(i = 0; i < N; i++) {
                if(readword(i+1) != 0) {
                        printf("\nNo no no ...\n");
                        return -1;
                }
        }
        printf("Good job!\n");
        readflag(1);
        return 0;
}
