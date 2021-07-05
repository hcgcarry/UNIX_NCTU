
# for(int i=8;i>=0;i--){
#     for(int j=0;j<=i;j++){
#         if(arr[j] > arr[j+1]) swap([arr[j],arr[j+1])
#     }
# }


mov eax,8
L1:
    cmp eax,0
    jl L1_exit
    mov ebx,0
        L2:
        cmp ebx,eax
        jg L2_exit

        mov ecx,[0x600000+4*ebx]
        mov edx,[0x600000+4*ebx+4]
        cmp ecx,edx
        jle L3
            mov [0x600000+4*ebx+4],ecx
            mov [0x600000+4*ebx],edx
        L3:

        inc ebx
        jmp L2
L2_exit:
        

    dec eax
    jmp L1
L1_exit:
done: