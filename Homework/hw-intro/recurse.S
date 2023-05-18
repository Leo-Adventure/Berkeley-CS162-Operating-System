	.file	"recurse.c"
	.text
	.section	.rodata
.LC0:
	.string	"i is %i. Address of i is %p\n"
	.text
	.globl	recur
	.type	recur, @function
recur:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	leaq	-4(%rbp), %rdx
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	-4(%rbp), %eax
	testl	%eax, %eax
	jle	.L2
	movl	-4(%rbp), %eax
	subl	$1, %eax
	movl	%eax, %edi
	call	recur
	jmp	.L3
.L2:
	movl	$0, %eax
.L3:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	recur, .-recur
	.ident	"GCC: (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
