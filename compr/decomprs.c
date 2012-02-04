/* Algorithm (c) Ed Ross. See RDCN.doc (bottom) for more info. */
/* Amiga implementation by Niklas Sjöberg (both C and .asm)
   reachable on 2:203/415.3@Fidonet

   After this comment, source is identical to that one in CUJ Oct 92.
*/

/* decompress inbuff_len bytes of inbuff into outbuff.
 * return length of outbuff.
 *
*/

//#include <kos.h>
//#include <string.h>
//#include <clib/exec_protos.h>

#include "../intro/data.h"

#define PROGRAM_BASE 0x0ef00000

#define CS_MASK 15
#define CS_LSL 4
/*
// DOES NOT WORK!!
// (and besides... the c-compiler generates smaller code ;))
void UnPackSLZ(unsigned char *inbuffer, unsigned char *outfile) {
	__asm__(
		"\n"
		"	mov	%0,r0\n"
		"	mov	%1,r1\n"
		"slz_loop:\n"
		"	mov.b	@r0+,r2\n"
		"	mov	#7,r3\n"
		"slz_loop2:\n"
		"	mov	#128,r7\n"	
		"	cmp/eq	r7,r2\n"
		"	bt	slz_comp\n"
		"	mov.b	@r0+,r4\n"
		"	mov.b	r4,@r1\n"
		"	add	#1,r1\n"
		"\n"
		"	add	r2, r2\n"
		"	add	#-1,r3\n"
		"	cmp/pz	r3\n"
		"	bt	slz_loop2\n"
		"	bra	slz_loop\n"
		"	nop\n"
		"\n"
		"slz_comp:\n"
		"	mov	#0,r4\n"
		"	mov.b	@r0+,r4\n"
		"	tst	r4, r4\n"
		"	bf	slz_exit\n"
		"\n"
		"	mov	#15,r5\n"
		"	and	r4,r5\n"
		"	add	#2,r5\n"
		"	shll2	r4\n"
		"	shll2	r4\n"
		"	mov.b	@r0+,r4\n"
		"\n"
		"	mov	r1,r6\n"
		"	sub	r4,r6\n"
		"\n"
		"slz_loop3:\n"
		"	mov.b	@r6+,r7\n"
		"	mov.b	r7,@r1\n"
		"	add	#1,r1\n"
		"	add	#-1,r5\n"
		"	cmp/pl	r5\n"
		"	bt	slz_loop3\n"
		"\n"
		"	add	r2, r2\n"
		"	add	#-1,r3\n"
		"	cmp/pz	r3\n"
		"	bt	slz_loop\n"
		"	bra	slz_loop2\n"
		"	nop\n"
		"slz_exit:\n"
		::"r"(inbuffer),"r"(outfile)
		:"r0","r1","r2","r3","r4","r5","r6","r7"
	);
}
*/
/*
void UnPackSLZ(unsigned char *inbuffer, unsigned char *outfile) {
	short myTAG, mycount;
	long int loop1;

	for(;;)  // loop forever (until goto occurs to break out of loop)
	{
		myTAG=*inbuffer++;
		for(loop1=0;(loop1!=8);loop1++) {
			if (myTAG&0x80) {
				if ((mycount=*inbuffer++)==0)  // Check EXIT
					{ goto skip2; } // goto's are gross but it's efficient :(
				else
				{
					int num = 2 + (mycount & CS_MASK);
					int buf2 = outfile;
					mycount <<= CS_LSL;
					mycount = mycount&0xff00 | (*inbuffer++);

					buf2 -= mycount;

					while (num != 0) {
						*outfile++ = *((unsigned char*)buf2++);
						num--;
					}
				}
			} else {
				*outfile++ = *inbuffer++;
			}
			myTAG+=myTAG;
		}
	}
skip2:
	return;
}
*/
/*
void UnPackSLZ(unsigned char *inbuffer, unsigned char *outfile) {
	int r0, r1, r2, r3, r4, r5, r6;

	r0 = inbuffer;
	r1 = outfile;

slz_loop:
	r2 = *inbuffer++;		// mov.b	@r0+,r2
	r3 = 7;				// mov		#7,r3
slz_loop2:
	r2 += r2;			// addv		r2, r2
	if (r2 & 256)
		goto slz_comp;		// bt		slz_comp;
	*(outfile++) = *inbuffer++;	// mov.b	@r0+,r4
					// mov.b	r4,@r1
					// add		#1,r1
	
	r3 += -1;			// add		#-1,r3
	if (r3 >= 0)			// cmp/pz	r3
		goto slz_loop2;		// bt		slz_loop2
	goto slz_loop;			// bra		slz_loop
					// nop

slz_comp:	
	r4 = *inbuffer++;		// mov.b	@r0+,r4
	if (r4 == 0)			// tst		r4, r4
		goto slz_exit;		// bf		slz_exit

	r5 = CS_MASK;			// mov.b	#CS_MASK,r5
	r5 &= r4;			// and		r4,r5
	r5 += 2;			// add		#2,r5
	r4 <<= CS_LSL;			// shll2	r4
					// shll2	r4
	r4 &= 0xff00;
	r4 |= (*inbuffer++);		// mov.b	@r0+,r4
			
	r6 = outfile;			// mov		r1,r6
	r6 -= r4;			// sub		r4,r6

slz_loop3:
	*(outfile++) = *((unsigned char *)r6++);
					// mov.b	@r6+,r7
					// mov.b	r7,@r1
					// add		#1,r1
	r5 += -1;			// add		#-1,r5
	if (r5 > 0)			// cmp/pl	r5
		goto slz_loop3;		// bt		slz_loop3
	
	r3 += -1;			// add		#-1,r3
	if (r3 >= 0)			// cmp/pz	r3
		goto slz_loop2;		// bt		slz_loop2
	goto slz_loop;			// bra		slz_loop
					// nop
slz_exit:
	return;				// rts
					// nop
}
*/
void dmain() {
	unsigned char *inbuffer = slz_data;
	unsigned char *outfile = (unsigned char*) (PROGRAM_BASE);

	short myTAG, mycount;
	long int loop1;

	// inlined UnPackSLZ
	for(;;)  // loop forever (until goto occurs to break out of loop)
	{
		myTAG=*inbuffer++;
		for(loop1=0;(loop1!=8);loop1++) {
			if (myTAG&0x80) {
				if ((mycount=*inbuffer++)==0)  // Check EXIT
					{ goto skip2; } // goto's are gross but it's efficient :(
				else
				{
					int num = 2 + (mycount & CS_MASK);
					int buf2 = outfile;
					mycount <<= CS_LSL;
					mycount = mycount&0xff00 | (*inbuffer++);

					buf2 -= mycount;

					while (num != 0) {
						*outfile++ = *((unsigned char*)buf2++);
						num--;
					}
				}
			} else {
				*outfile++ = *inbuffer++;
			}
			myTAG+=myTAG;
		}
	}
skip2:

	asm("jmp @%0" :: "r" (PROGRAM_BASE));
}

