/* ===================================================================================
 *  samplegen.c - samplegenerator routines (c) Outbreak 2001-2002
 * ===================================================================================
 *
 * @brief	:	samplegenerator routines
 * @author	:	Fredrik "quarn" Ehnbom
 */

#include "samplegen.h"

// -----------------------------------------------------------------------------------
//  ssynth_square - generates a squarewave-sample
//                  TODO: this probably can be written much better
// -----------------------------------------------------------------------------------

void ssynth_square(
	double fstart, double vol,
	double freqs[2][2], double vols[2][3], unsigned char wavelen,
	short *samples
) {
	int freqPos = 0;
	double freq2 = fstart;
	double freqPitch = freqs[0][freqPos];
	int freqLen = (int) freqs[1][freqPos];

	int volPos = 0;
	double volPitch = vols[0][volPos];
	int volLen = (int) vols[1][volPos];

	double pos = 0;
	double freqlen = 44100 / freq2;
	double wconstant = (wavelen / 100.0f);
	double wavelen2 = wconstant * freqlen;

	unsigned char done = 0;
	int constant = -1;

	int i;

	for (i = 0; i < 44100; i++) {
		samples[i] = constant * vol;

		if ((pos > wavelen2 && !done) || pos > freqlen) {
			constant = -constant;

			if (pos > freqlen) {
				pos = 0;
				done = 0;
			} else {
				done = 1;
			}
		}

		pos ++;
		vol += volPitch;
		freq2 += freqPitch;
		freqlen = 44100 / freq2;
		wavelen2 = wconstant * freqlen;

		if (--volLen < 0) {
			volPos++;
			volPitch = vols[0][volPos];
			volLen = (int) vols[1][volPos];
		}
		if (--freqLen < 0) {
			freqPos++;
			freqPitch = freqs[0][freqPos];
			freqLen = (int) freqs[1][freqPos];
		}
	}
}
