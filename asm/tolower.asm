mov AL,[0x600000]
lea ebx,[32]
not BL
and AL,BL
mov [0x600001],AL