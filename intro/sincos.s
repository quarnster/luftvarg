	.globl _sin, _cos

	.text
_sin:
	mova scale, r0
	fmov @r0, fr0
	fmul fr0, fr4	
	ftrc fr4, fpul          !  fpul = (int) (fr4 * 32768 / PI )
	rts
	.word 0xf0fd
!	fsca fpul, dr0
_cos:
	mova scale, r0
	fmov @r0, fr0
	fmul fr0, fr4
	ftrc fr4, fpul          !  fpul = (int) (fr4 * 32768 / PI )
	.word 0xf0fd
!	fsca fpul, dr0
	rts
	fmov fr1, fr0

.align 4	
scale:
	.float	10430.37835     ! 32768 / PI

	.end

	

