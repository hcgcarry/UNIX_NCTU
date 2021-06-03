PROGS = libmini.so start.o
all: $(PROGS)

libmini.so:libmini64.asm libmini.c
	yasm -f elf64 -DYASM -D__x86_64__ -DPIC libmini64.asm -o libmini64.o
	gcc -c -g -Wall -fno-stack-protector -fPIC -nostdlib libmini.c
	ld -shared -o libmini.so libmini64.o libmini.o

start.o:start.asm
	yasm -f elf64 -DYASM -D__x86_64__ -DPIC start.asm -o start.o

%:%.c start.o libmini.so
	gcc -c -g -Wall -fno-stack-protector -nostdlib -I. -I.. -DUSEMINI $<
	ld -m elf_x86_64 --dynamic-linker /lib64/ld-linux-x86-64.so.2 -o $@ $@.o start.o -L. -L.. -lmini

clean:
	rm $(PROGS)