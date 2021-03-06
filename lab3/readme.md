# Extend the Mini Lib C to Handle Signals
In this homework, you have to extend the mini C library introduced in the class to support signal relevant system calls. You have to implement the following C library functions in Assembly and C using the syntax supported by yasm x86_64 assembler.

1. setjmp: prepare for long jump by saving the current CPU state. In addition, preserve the signal mask of the current process.
2. longjmp: perform the long jump by restoring a saved CPU state. In addition, restore the preserved signal mask.
3. signal and sigaction: setup the handler of a signal.
4. sigprocmask: can be used to block/unblock signals, and get/set the current signal mask.
5. sigpending: check if there is any pending signal.
6. alarm: setup a timer for the current process.
7. functions to handle sigset_t data type: sigemptyset, sigfillset, sigaddset, sigdelset, and sigismember.

The API interface is the same to what we have in the standard C library. However, because we are attempting to replace the standard C library, our test codes will only be linked against the library implemented in this homework. We will use the following commands to assemble, compile, link, and test your implementation.
```
$ yasm -f elf64 -DYASM -D__x86_64__ -DPIC libmini64.asm -o libmini64.o
$ gcc -c -g -Wall -fno-stack-protector -fPIC -nostdlib libmini.c
$ ld -shared -o libmini.so libmini64.o libmini.o
$ yasm -f elf64 -DYASM -D__x86_64__ -DPIC start.asm -o start.o
$ gcc -c -g -Wall -fno-stack-protector -nostdlib -I. -I.. -DUSEMINI test.c
$ ld -m elf_x86_64 --dynamic-linker /lib64/ld-linux-x86-64.so.2 -o test test.o start.o -L. -L.. -lmini
```
Where *libmini64.asm* and *libmini.c* is your extended Assembly and C implmentation for this homework, respectively, and *test.c* is the testing code prepared by the TAs. The *start.o* file is the program start routine we have introduced in the class. Please notice that there is a -nostdlib parameter passed to the compiler, which means that you could not use any existing functions implemented in the standard C library. Only the functions you have implemented in the *libmini64.asm* and *libmini.c* file can be used. In addition to your library source code, you also have to provide a corresponding *libmini.h* file. The testing codes will include this file and use the function prototypes and data types defined in the header file.

To ensure that your library can handle signals properly, your implemented setjmp function must save the signal mask of the current process in your customized jmp_buf data structure. The saved signal mask must be restored by the longjmp function. This requirement is different from the default setjmp/longjmp implementation.

## Hints
This might be a relatively difficult homework, so we have several hints for this homework. Reading these hints carefully may reduce your efforts to implement this homework.
1. x86_64 system call table: It should be easy for you to find one on Internet. Here is the one we demonstrated in the course ([x86_64 syscall table](http://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/)).
2. If you need precise prototypes for system calls, you may refer to an online Linux cross reference (LXR) site. For example, this page shows the official prototypes form the linux kernel ([include/linux/syscalls.h](https://elixir.bootlin.com/linux/v4.16.8/source/include/linux/syscalls.h#L603)).
3. You will have to define all the required data structures and constants by yourself. If you do not know how to define a data structure, try to find them from the Linux kernel source codes.
4. With LXR, you may also check how a system call is implemented, especially when an error code is returned from a system call. For example, here is the implementation for [sys_rt_sigaction](https://elixir.bootlin.com/linux/v4.16.8/source/kernel/signal.c#L3711) system call in the kernel. By reading the codes, you would know that passing an incorrect sigset_t size would lead to a negative EINVAL error code.
5. For implementing setjmp with a preserved process signal mask, the recommended data structure for x86_64 is given below:
    ```
    typedef struct jmp_buf_s {
        long long reg[8];
        sigset_t mask;
    } jmp_buf[1];
    ```
    The minimal eight 64-bit values you have to preserve in the reg array are: RBX, RSP, RBP, R12, R13, R14, R15, and the return address (to the caller of setjmp). The current process signal mask can be preserved in the mask field.
6. To ensure that a signal handler can be properly called without crashing a process, you have to do the following additional setup in your implemented sigaction function as follows (illustrated in C language):
    ```
    long sigaction(int how, const struct sigaction *nact, struct sigaction *oact) {
        ...
        nact->sa_flags |= SA_RESTORER;
        nact->sa_restorer = /* your customized restore routine, e.g., __myrt */;
        ret = sys_rt_sigaction(how, nact, oact, sizeof(sigset_t));
        ...
    }
    ```
    The implementation of the __myrt function is simply making a system call to sigreturn (rax = 15).
7. Please notice that the sigaction data structure used in the C library may be different from that used in the kernel. If the user space and the kernel space data structure are inconsistent, you will have to perform the convertion in your wrapper functions.
8. The assembly instructions we used in this homework are pretty simple. All the required instructions have been introduced in the class. Here we enumerate the possible required assembly instructions for this homework.
```
add     call	cmp	    inc	    jge	    jmp	    jne
jnz	    lea	    mov	    neg	    or	    pop	    push
ret	    sub	    syscall	xor
```

## debug
* sigset_t ????????? ,?????????__val[32],??????sys_rt_sigpending ???invalid argument
* sys_sigreturn: ????????????system call ??????macro ??????run alarm3??????????????????SEGV fault, ?????????signal ???????????????,???????????????????????????????????????,????????????,sys_sigreturn??????rsp??????signal handler??????????????????,??????????????????pop ????????????
