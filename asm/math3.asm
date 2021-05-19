mov eax,[0x600000]
lea ebx,[5]
mul ebx
lea edx,[0]
mov ecx,[0x600004]
lea ecx,[ecx-3]
div ecx
mov [0x600008],eax