mov eax,[0x600000]
neg eax
mul dword PTR [0x600004]
cdq

mov ecx,[0x600008]
sub ecx,ebx
idiv ecx

mov [0x600008],eax
done: