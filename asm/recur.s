
section .text

global _start

_start:
    mov rdi,5
    call r
    

	mov	rax, 60
	mov	rdi, 0		; code
	syscall			; syscall: exit
    ret



r:
    push rdi
    cmp rdi,0
    jg L1
    mov rax,0
    pop rdi
    ret

    L1:
    cmp rdi,1
    jne L2

    mov rax,1
    pop rdi
    ret

    L2:
    sub rdi,1
    call r
    lea rax,[2*rax]

    pop rdi
    push rax
    sub rdi,2
    call r
    lea rax,[rax+rax*2]

    pop rbx
    add rax,rbx

    ret

    


