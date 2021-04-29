#include "tools.c"

int
main(int argc, char *argv[]) {
        void *h;
        void (*orw)();
        int syscalls[] = { SCMP_SYS(open), SCMP_SYS(close), SCMP_SYS(openat), SCMP_SYS(read), SCMP_SYS(write), SCMP_SYS(exit), SCMP_SYS(exit_group) };

        CHECKARG(argc, 2, "usage: orw /path/to/your/file.so\n");
        LOAD_SO(h, argv[1]);
        LOAD_SYM(orw, h, "orw");

        unbuffered();

        allow_syscall(syscalls, sizeof(syscalls)/sizeof(int));
        orw();

        return 0;
}

