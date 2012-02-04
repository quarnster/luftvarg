
	! Dreamcast Video example by marcus
	!
	! Set up a simple screen and
	! draw some text
	!
	! To compile into SREC using GNU binutils:
	!
	! sh-elf-as -little -o video.o video.s
	! sh-elf-ld -EL --oformat srec -Ttext 0x8c010000 video.o -o video.srec


	.globl	start, _memset, _memcpy
	
	.text
start:
	! First, make sure to run in the P2 area
	mov.l	setup_cache_addr,r0
	mov.l	p2_mask,r1
	or	r1,r0
	jmp	@r0
	nop
setup_cache:
	! Now that we are in P2, it's safe
	! to enable the cache
	mov.l	ccr_addr,r0
	mov.w	ccr_data,r1
	mov.l	r1,@r0
	! After changing CCR, eight instructions
	! must be executed before it's safe to enter
	! a cached area such as P1
	mov.l	main_addr,r0	! 1
	mov	#0,r1		! 2
	nop			! 3
	nop			! 4
	nop			! 5
	nop			! 6
	nop			! 7
	nop			! 8
	jmp	@r0		! go
	mov	r1,r0

	.align	4
p2_mask:
	.long	0xa0000000
setup_cache_addr:
	.long	setup_cache
main_addr:
	.long	main
ccr_addr:
	.long	0xff00001c
ccr_data:
	.word	0x090b

	! Real program starts here.
main:
	mov.l	r14,@-r15
	sts	pr,r14

	! Jump to the kernel main
	nop

	mov.l	c_main_addr,r0
	jsr	@r0
	nop

	lds	r14,pr
	rts
	mov.l	@r15+,r14
c_main_addr:
	.long	_dmain
!program_base:
!	.long	0x0ef00000
