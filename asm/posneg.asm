cmp eax,0
jl L1_2
mov dword ptr [0x600000],1
jmp L1_exit
L1_2:
mov dword ptr [0x600000],-1
L1_exit:

cmp ebx,0
jl L2_2
mov dword ptr [0x600004],1
jmp L2_exit
L2_2:
mov dword ptr [0x600004],-1
L2_exit:

cmp ecx,0
jl L3_2
mov dword ptr [0x600008],1
jmp L3_exit
L3_2:
mov dword ptr [0x600008],-1
L3_exit:

cmp edx,0
jl L4_2
mov dword ptr [0x60000c],1
jmp L4_exit
L4_2:
mov dword ptr [0x60000c],-1
L4_exit: