/* ===================================================================================
 *  main.c - Musikroutines for the aica chip (c) Outbreak 2002
 * ===================================================================================
 *
 * @brief	:	music routines for "Airwolf 4k"
 * @author	:	Fredrik "quarn" Ehnbom
 */

#include "data.h"

int __gccmain() {}

#define TICKCOUNT	75
#define TIMER		0x00010000
#define SAMPLE_BASE	0x00010004

#define AICA(n) ((volatile unsigned int *)(void*)(0x800000+(n)))

typedef unsigned char uint8;
typedef unsigned long uint32;

typedef struct {
	uint32 dataOff;
	uint32 length;
} Pattern;

// -----------------------------------------------------------------------------------
//  aica_reset - reset all the 64 channels to a silent state
// -----------------------------------------------------------------------------------

void aica_reset() {
	uint32 i, j;
	volatile uint32 *hwptr = AICA(0);

	*AICA(0x2800) = 0;

	// Reset all 64 channels to a silent state
	for (i = 0; i < 64; i++) {
		hwptr[0] = 0x8000;
		hwptr[5] = 0x1f;
		hwptr[1] = 0;
		hwptr[2] = 0;
		hwptr[3] = 0;
		hwptr[4] = 0;
		for (j = 6; j < 32; j++)
			hwptr[j] = 0;
		hwptr += 32;
	}

	*AICA(0x2800) = 15;
}

// -----------------------------------------------------------------------------------
//  stop_sound - Disable playback for the specified channel
// -----------------------------------------------------------------------------------

void stop_sound(int channel) {
	volatile uint32 *hwptr = AICA(channel<<7);

	// Disable playback
	hwptr[0] = (hwptr[0] & ~0x4000) | 0x8000;
}

// -----------------------------------------------------------------------------------
//  play_sound - play the sound for the specified channel in the specified frequence
// -----------------------------------------------------------------------------------

void play_sound(int channel, unsigned long freq) {
	volatile uint32 *hwptr = AICA((channel)<<7);
	uint32 freq_lo, freq_base = 5644800;
	int freq_hi = 7;

	stop_sound(channel);
	// Set sample format and buffer address
	hwptr[0] = /*0x4000 | */(((unsigned long)SAMPLE_BASE + 2 * 44100 * (channel%3))>>16);
	hwptr[1] = ((unsigned long)(SAMPLE_BASE + 2 * 44100 * (channel%3))) & 0xffff;
	// Number of samples
	hwptr[3] = 44100;
	// Need to convert frequency to floating point format
	// (freq_hi is exponent, freq_lo is mantissa).
	// Formula is  freq = 44100*2^freq_hi*(1+freq_lo/1024)
	//
	while (freq < freq_base && freq_hi > -8) {
		freq_base >>= 1;
		--freq_hi;
	}
	freq_lo = (freq<<10)/freq_base;
	// Write resulting values
	hwptr[6] = (freq_hi<<11)|(freq_lo&1023);
	// Set volume, pan, and some other stuff
	((volatile unsigned char *)(hwptr+9))[4] = 0x24;
	((volatile unsigned char *)(hwptr+9))[1] = 0xf;
	
	hwptr[4] = 0x1f;
	// Enable playback
	hwptr[0] |= 0xc000;
}

// -----------------------------------------------------------------------------------
//  main routine
// -----------------------------------------------------------------------------------

void main() {
	extern volatile uint32 timer;

	uint32 lastUpdated = 0;
	uint32 dataPos = 0;
	uint8 *patternOrder;
	uint32 i;
	uint32 tmp;
	unsigned long table[48];
	
	Pattern *pattern;

	uint8 songLength = data[dataPos++];
	uint8 patternLength = data[dataPos++];

	// allocate dynamic memory (no, I won't take the
	// time (and space) to write a malloc)
	patternOrder = (uint8*) (4000);
	pattern = (Pattern*) (4000 + songLength);

	aica_reset();

	// get patternorder
	for (i = 0; i < songLength; i++) {
		if (!(i%2)) {
			tmp = data[dataPos++];
			patternOrder[i] = tmp >> 4;
			continue;
		}
		patternOrder[i] = tmp & 0x0f;
	}

	// read patterns
	for (i = 0; i < patternLength; i++) {
		pattern[i].length = data[dataPos++];
		pattern[i].dataOff = dataPos;
		dataPos += pattern[i].length;
	}

	// create frequency table
	{
		// very bad precision :)
		table[0] = 8363;
		uint32 curr = 8363 * 10594 / 10000;
		for (i = 1; i < 48; i++) {
			table[i] = curr;
			curr = curr * 10594 / 10000;
		}
	}

	i = 0;
	tmp = patternOrder[i];
	dataPos = 0;
	timer = 0;

	// song loop	
	while (1) {
		// wait for tick
		while (timer - lastUpdated < TICKCOUNT) *((uint32*) (TIMER)) = timer;
		lastUpdated = timer;
		
		uint32 check = data[pattern[tmp].dataOff + dataPos++];

		// channel 1
		if (check & 0x80) {
			if (check & 2) play_sound(1, 31574); // 31574.483507828012
			else stop_sound(1);
		
		}

		// channel 2
		if (check & 0x40) {
			if (check & 4) stop_sound(0);
			else if (check & 8) play_sound(0, 8363);
			else play_sound(0, table[data[pattern[tmp].dataOff + dataPos++]-49]);;
		}

		// channel 3
		if (check & 0x20) {
			if (check & 1) play_sound(2, 44653); // 44653.06280169598
			else stop_sound(2);
		}

		// channel 4
		if (check & 0x10) {
			uint32 note = data[pattern[tmp].dataOff + dataPos++];
			if (note == 97) stop_sound(3);
			else play_sound(3, table[note-49]);
		}

		// check if we should change pattern
		if (dataPos == pattern[tmp].length) {
			i++;
			if (i == songLength) break;
			tmp = patternOrder[i];

			dataPos = 0;
		}
	}

	aica_reset();
}
