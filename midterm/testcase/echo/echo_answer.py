from pwn import *


r = remote('echo',8000)

for i in range(1000):
    z = r.recvuntil('enter: ').decode('utf-8').split()
    r.sendline(z[-6].split("'")[1])

r.interactive()
