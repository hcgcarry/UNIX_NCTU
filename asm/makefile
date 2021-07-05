%:%.s
	yasm -f elf64 -DYASM -D__x86_64__ -DPIC $< -o $@.o
	ld -m elf_x86_64 -o $@ $@.o