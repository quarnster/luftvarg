/* ===================================================================================
 *  main.c - Main for the airwolf 4k (c) Outbreak 2002
 * ===================================================================================
 *
 * @brief	:	main for "Airwolf 4k"
 * @author	:	Fredrik "quarn" Ehnbom
 */
#include "../aica/music.h"
#include "samplegen.h"
#include "sincos.h"

/* aica stuff */
#define AICA_BASE   0xa0800000
#define TIMER       0xa0810000
#define SAMPLE_BASE 0xa0810004

extern unsigned char *music;

/* video stuff */
#define WIDTH 320
#define HEIGHT 240
#define SCREENSIZE WIDTH * HEIGHT

#define VIDEO_BASE 0xa5000000

#define MAX_Y  500

void memcpy4(void* tgt, void *src, int length);
void memset4(void* tgt, int bleh, int length);

unsigned char seed = 0x12;

unsigned char rand() {
	unsigned short tmp = (0x4e6d) * seed;
	seed = (tmp & 0xff) + 0x39;

	return ((tmp >> 8) + 0x30);
}

// -----------------------------------------------------------------------------------
//  qmain - main routine for "airwolf 4k"
// -----------------------------------------------------------------------------------

void qmain() {
	int x;
	double freqs[2][2];
	double vols[2][3];

	signed short *sample = (signed short*) (SAMPLE_BASE);

	// -----------------------------------------------------------------------------------
	//  init_video - inits a 320*240 graphicsmode.
	//		 TODO: test it on a tv...
	// -----------------------------------------------------------------------------------

	{
		unsigned long * videobase = (unsigned long *)0xa05f8000;
		unsigned long * cvbsbase = (unsigned long *)0xa0702c00;
		int disp_lines, size_modulo, il_flag;

		// Looks up bytes per pixel as a shift value
		unsigned long bppshift = 1; // bppshifttab[pixel_mode];
		int cable_type;
		int pixel_mode;

		// inlined vid_check_cable
		{
			unsigned long * porta = (unsigned long *)0xff80002c;
	
			*porta = (*porta & 0xfff0ffff) | 0x000a0000;
	
			// Read port8 and port9
			cable_type =  (*((unsigned short*)(porta + 1)) >> 8) & 3;
		}


		// Save the video memory size

		// Set border color
//		*(videobase + 0x10) = (255 << 16) | (0 << 8) | (255 << 0);

		// Set pixel clock and color mode
		pixel_mode = (0 << 2) | 1;	// Color mode and SD/DE
		disp_lines = (240 / 2) << 1;
		if (cable_type == 0) {
			// Double # of display lines for VGA and S-Video
			disp_lines <<= 1;		// Double line count
			pixel_mode |= 0x800002;		// Set clock double
		}
		*(videobase + 0x11) = pixel_mode;
		*(videobase + 0x3a) |= 0x100;		// Horizontal SD

		// Set video base address (right at the top)
		*(videobase + 0x50) = 0;
		*(videobase + 0x54) = 0 + (320 << bppshift);


		// Set screen size and modulo, and interlace flag
		il_flag = 0x100;	// Set Video Output Enable
		size_modulo = 0;

		*(videobase + 0x17) = ((size_modulo+1) << 20) | ((disp_lines-1) << 10) | (((320/4) << bppshift) - 1);
		*(videobase + 0x34) = il_flag;

		// Set vertical pos and border
		*(videobase + 0x37) = (24 << 16) | (24 + disp_lines);
		*(videobase + 0x3c) = (24 << 16) | 24;

		// Horizontal pos
		*(videobase + 0x3b) = 0xa4;
	
		// Set the screen size
		if (cable_type != 0) // this does not work with vga?
			*(videobase + 0x36) = (262<<16) | (857 << 0);

		// Select RGB/CVBS
		if (cable_type == 3)
			*cvbsbase = 3 << 8;
		else
			*cvbsbase = 0;
	}

	// -----------------------------------------------------------------------------------
	//  init_aica - generates samples and inits the aica
	// -----------------------------------------------------------------------------------

	{
		// Pull RESET low on ARM
		*((volatile unsigned long *)(void *)0xa0702c00) |= 1;

		// Clear entire sound RAM
		memset4((void*)AICA_BASE, 0, 2*1024*1024);

		// Copy ARM code to beginning of sound RAM
		memcpy4((void*)AICA_BASE, aica_data, sizeof(aica_data));

		// melody
		freqs[0][0] = 0; freqs[1][0] = 44101;

		vols[0][0] = -0.1413; vols[1][0] = 19110;
		vols[0][1] = -0.8009; vols[1][1] = 9739;
		vols[0][2] =  0.0000; vols[1][2] = 15252;

		ssynth_square(
			440, 10500,
			freqs, vols, 50,
			sample
		);

		// bassdrum
		sample = (signed short*) (SAMPLE_BASE + 44100 * 2);

		freqs[0][0] = -0.0962; freqs[1][0] = 7530;
		freqs[0][1] =  0.0000; freqs[1][1] = 36570;

		vols[0][0] =  0.0000; vols[1][0] = 8636;
		vols[0][1] = -60.4082; vols[1][1] = 180;
		/*vols[0][2] =  0.0000;*/ vols[1][2] = 35284;

		ssynth_square(
			725,11000,
			freqs, vols, 100,
			sample
		);

		// snare
		sample = (signed short*) (SAMPLE_BASE + 44100 * 4);

		freqs[0][0] = -2.6870; freqs[1][0] = 735;
		/*freqs[0][1] =  0.0000;*/ freqs[1][1] = 43365;

		vols[0][0] =  128.9796; vols[1][0] = 184;
		vols[0][1] = - 16.1224; vols[1][1] = 1470;
		/*vols[0][2] =    0.0000;*/ vols[1][2] = 42446;

		ssynth_square(
			1975,0,
			freqs, vols, 50,
			sample
		);
	}

	// -----------------------------------------------------------------------------------
	//  generate heightmap
	// -----------------------------------------------------------------------------------

	static unsigned char map[256*256];
	memset4(map, 0x0, 256*256);

	for (x = 0; x < 200; x++) {
		int x1 = rand() * rand()/2;
		int y1 = rand() * rand();
		int r = 100 + rand() / 10;
//		hill(rand() * rand()/2, rand() * rand(), 100 + rand() / 10);
		{
			int x;
			int y;

			int x2 = x1 - r/2;
			int y2 = y1 - r/2;

			for (y = 0; y < r; y++, y2++) {
				for (x = 0; x < r; x++) {
					int pos = ((y2)&255) * 256 + ((x2+x)&255);
					int val;

					float y3 = (y2 - y1) / (float) (r / 2);
					float x3 = (x2+x - x1) / (float) (r / 2);
					float z = 256 - 1024* (x3 * x3 + y3 * y3);
					z  /= 15;
					z = z < 0 ? 0 : z > 255 ? 255 : z;

					val = map[pos] + (int) z;
					val = val < 0 ? 0 : val > 255 ? 255 :  val;

					map[pos] = val;
				}
			}
		}
	}

	// Release RESET which will cause ARM to start executing
	*((volatile unsigned long *)(void *)0xa0702c00) &= ~1;

	// Wait for a little bit so we know that the ARM has done its
	// basic initialization.
	for (x = 0; x < 0x200000; x++);
	
	unsigned int timer = 0;
	unsigned int last = 0;
	unsigned int last2 = 57550-450;
	unsigned int last3 = 0;

	unsigned char curpage = 0;

	int posx = 0;
	int posy = 0;
	
	int y;
	int i;

	int clipMin = 248;
	int clipMax = 239;
	
	static unsigned short *buffer = (unsigned short*) (VIDEO_BASE + SCREENSIZE*2);

	while (timer < 64000) {
		timer = *(unsigned int*)(TIMER);

		if (timer - last > 450 && clipMin != 0) {
			last += 450;
			clipMin -= 8;
		} else if (timer > 57550 && (timer - last2) > 450 && clipMax != 0) {
			last2 += 450;
			clipMax -= 19;
		}

		// -----------------------------------------------------------------------------------
		//  effect stuff
		// -----------------------------------------------------------------------------------

		int pz = 0;
		int px = 0;

		// angle of rotation
		float arg = sin(timer*0.0005);
		int cos256 = 33554 * cos(arg);
		int sin256 = 33554 * sin(arg);

		memset4(buffer, 0, SCREENSIZE * 2);

		for (x = 0; x < WIDTH; x++) {
			int drawYmax = clipMin;

			pz =  ((x * cos256)) - (sin256>>1);
			px = -((x * sin256)) - (cos256>>1);


			for (y = MAX_Y+1; --y >= 0; px += cos256, pz += sin256) {
				int val = (map[(((px >> 16)+(posx>>13))&0xff) + ((((pz >> 16)+(posy>>13))&0xff) << 8)]) + ((MAX_Y - y)>>1) - 128;

				if (val > clipMax) val = clipMax;

				if (val > drawYmax) {
					int i;
					for (i = drawYmax; i < val; i++) {
						buffer[(239 - i)* 320	 + x] = (255 - ((MAX_Y-y)>>1))>>3;
					}

					drawYmax = val;
					if (drawYmax == clipMax) break;
				}
			}
		}
		int bleh = ((timer - last3) >> 3)>>2;
		posx += cos256 * bleh;
		posy += sin256 * bleh;
		last3 = timer;

		// -----------------------------------------------------------------------------------
		//  double buffering
		// -----------------------------------------------------------------------------------

		volatile unsigned long *vbl = ((unsigned long *)0xa05f810c);
		volatile unsigned int *reg = (volatile unsigned int *)(void*)0xa05f8050;

		// wait for the beam to go offscreen
		while (!(*vbl & 0x01ff));
		// Switch to the previously rendered screen
		unsigned int addr = ((unsigned int)(!curpage ? VIDEO_BASE : (VIDEO_BASE +  SCREENSIZE*2)))&0x007fffff;
		reg[0] = addr;
		reg[1] = addr+320*2;

		buffer = (unsigned short*) (curpage ? VIDEO_BASE : (VIDEO_BASE +  SCREENSIZE*2));

		// wait for the beam to go onscreen again
		while (*vbl & 0x01ff);

		curpage ^= 1;
	}
}
