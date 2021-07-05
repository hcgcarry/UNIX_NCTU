import binascii
from pwn import * 
import sys
r = remote('up.zoolab.org',2530)


while 1:
    try:
        code_bytes = r.recvuntil('Your answer: ').decode('utf-8').split()

        code_bytes = code_bytes[-3]
        # print(code_bytes)

        z = binascii.a2b_hex(code_bytes)


        from capstone import *


        md = Cs(CS_ARCH_X86, CS_MODE_64)
        result=''

        for i in md.disasm(z, 0x1000):
            result = result + "{} {}\n".format(i.mnemonic, i.op_str)

        # print("result",result)
        tmp = binascii.b2a_hex(result.encode("ascii")).decode('ascii')
        # print(tmp)
        r.sendline(tmp)
    except Exception as e:
        r.interactive()


