mov ebx,[0x600000]
neg ebx
lea ebx,[ebx+ebx*4]


mov eax,[0x600004]
neg eax
cdq 
mov ecx,[0x600008]
idiv ecx
mov esi,edx

mov eax,ebx
cdq 
idiv esi

mov [0x60000c],eax
done: