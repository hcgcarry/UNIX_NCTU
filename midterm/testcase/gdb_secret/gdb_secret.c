 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <crypt.h>

#include "tools.c"

static char secret[] = "$6$XnjDnFlrRdjJ78AR$XA6PWaEeKuWQyWXKjqosBLmVdMTX6Xl03jiYJQ0G9LSKdF.vWeX.nr2bLShLfrO63lSyD11mLjKHWtpa0cLbb.";

int main() {
        int r;
        char buf[1024], *token, *out;

        unbuffered();

        printf("Show me the secret: ");
        if(fgets(buf, sizeof(buf), stdin) == NULL) return -1;
        if((token = strtok(buf, " \t\n\r")) == NULL) {
                fprintf(stderr, "Invalid input.\n");
                return -1;
        }
        if((out = crypt(token, secret)) == NULL) {
                perror("crypt");
                return -1;
        }
        r = strncmp(out, secret, sizeof(secret));
        if(r == 0) {
                printf("Bingo!\n");
                readflag(0);
        } else {
                printf("%s\n", out);
                printf("Nope!\n");
        }
        return 0;
}
