#include <stdio.h>
#include <dlfcn.h>
#include <seccomp.h>
#include "tools.c"

static void
setup_filter() {
        static int reentrant = 0;
        if(reentrant) return;
        reentrant++;

        scmp_filter_ctx ctx;
        /* only the following syscalls are allowed:
           open read write exit exit_group */
        if((ctx = seccomp_init(SCMP_ACT_ALLOW)) == NULL) xerror("seccomp_init");
        if(seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(clone), 0) < 0)        xerror("seccomp_rule");
        if(seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(fork), 0) < 0)         xerror("seccomp_rule");
        if(seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(vfork), 0) < 0)        xerror("seccomp_rule");
        if(seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(execve), 0) < 0)       xerror("seccomp_rule");
        if(seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(execveat), 0) < 0)     xerror("seccomp_rule");
        if(seccomp_load(ctx) < 0) xerror("seccomp_load");
}

void help() {
        printf( "list of available commands:\n"
                " - show symbol: show the address of a function\n"
                " - call symbol: make a function call\n"
                " - read address: read 8-byte value from [address]\n"
                " - write address value: write 8-byte [val] to memory [address]\n"
                " - timestamp: show current timestamp\n"
                " - help: show this message\n"
                " - quit: quit from the service\n"
                "");
}

#define INVALID()       { printf("** bad argument\n"); return; }

void *me = NULL;

void show(const char *sym) {
        void *s;
        if(sym == NULL) INVALID();
        if((s = dlsym(me, sym)) != NULL) {
                printf("%s = %p\n", sym, s);
        } else {
                printf("** symbol not found\n");
        }
}

void call(const char *sym) {
        void (*s)(void*, void*, void*, void*, void*, void*, void*, void*, void*, void*, void*, void*);
        if(sym == NULL) INVALID();
        if(strcmp(sym, "bingo") == 0) goto no_bingo;
        if((s = dlsym(me, sym)) != NULL) {
                s(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        } else {
                printf("** symbol not found\n");
        }
        return;
no_bingo:
        printf("** you cannot call bingo!\n");
}
vvoid bingo() {
        printf("Bingo!");
        readflag(1);
}

int main() {
        static int reentrant = 0;
        if(reentrant) return 0;
        reentrant++;

        setup_filter();
        unbuffered();

        printf("Welcome to the Self-Inspection Service\n");

        if((me = dlopen(NULL, RTLD_LAZY)) == NULL) xerror("dlopen");

        while(1) {
                char buf[128], *cmd, *arg1, *arg2, *saveptr;
                printf("\nprompt> ");
                if(fgets(buf, sizeof(buf), stdin) == NULL) break;
#define DELIM " \t\n\r"
                if((cmd = strtok_r(buf, DELIM, &saveptr)) == NULL) continue;
                arg1 = strtok_r(NULL, DELIM, &saveptr);
                arg2 = strtok_r(NULL, DELIM, &saveptr);

                if(strcmp(cmd, "help") == 0)            help();
                else if(strcmp(cmd, "show") == 0)       show(arg1);
                else if(strcmp(cmd, "call") == 0)       call(arg1);
                else if(strcmp(cmd, "read") == 0)       memread(arg1);
                else if(strcmp(cmd, "write") == 0)      memwrite(arg1, arg2);
                else if(strcmp(cmd, "timestamp") == 0)  timestamp();
                else if(strcmp(cmd, "quit") == 0)       break;
                else { printf("** unknown command: %s\n", cmd); }
        }

        printf("Thank you for using our service.\n");

        return 0;
}


