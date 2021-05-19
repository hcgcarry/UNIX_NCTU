

	section .text
	global _start
_start:
	mov eax,0xdc795238
	add eax,0xe66d8bcb
	sub eax,0x73da745a
	mov	rax, 1
	mov 	rdi, 1		; stdout
	mov	rdx, 14		; length
	syscall			; syscall: write

	mov	rax, 60
	mov	rdi, 0		; code
	syscall			; syscall: exit
	ret
