#ifndef __INCLUDED_SAMPLEGEN_H
#define __INCLUDED_SAMPLEGEN_H

// #include "types.h"

void ssynth_square(
	double fstart, double vstart,
	double freq[2][2], double fakevol[2][3],  unsigned char wavelen,
	short *samples
);


#endif
