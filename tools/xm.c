#include "xm.h"

typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;

uint16 _convert16(unsigned char *data) {
	return (uint16) (data[0] | data[1] <<8);
//	return _convert16(data, 0);
}
/*
uint16 _convert16(unsigned char *data, uint32 off) {
	;
}
*/
static char notes[100][3] = {
		"???",
		"C-0", "C#0", "D-0", "D#0", "E-0", "F-0", "F#0", "G-0", "G#0", "A-0", "A#0", "B-0",
		"C-1", "C#1", "D-1", "D#1", "E-1", "F-1", "F#1", "G-1", "G#1", "A-1", "A#1", "B-1",
		"C-2", "C#2", "D-2", "D#2", "E-2", "F-2", "F#2", "G-2", "G#2", "A-2", "A#2", "B-2",
		"C-3", "C#3", "D-3", "D#3", "E-3", "F-3", "F#3", "G-3", "G#3", "A-3", "A#3", "B-3",
		"C-4", "C#4", "D-4", "D#4", "E-4", "F-4", "F#4", "G-4", "G#4", "A-4", "A#4", "B-4",
		"C-5", "C#5", "D-5", "D#5", "E-5", "F-5", "F#5", "G-5", "G#5", "A-5", "A#5", "B-5",
		"C-6", "C#6", "D-6", "D#6", "E-6", "F-6", "F#6", "G-6", "G#6", "A-6", "A#6", "B-6",
		"C-7", "C#7", "D-7", "D#7", "E-7", "F-7", "F#7", "G-7", "G#7", "A-7", "A#7", "B-7",
		"[-]"
};


void _xm_convertPattern(FILE *in, FILE *out) {
		uint8 *data = (uint8*) malloc(10);
		uint8 *data2;
		uint8 *oData;
		uint8 oDataSize = 0;
		uint16 rows;
		uint16 size;
		int patternpos = 0;
		int i;
		
		// header
		fread(data, 1, 4, in);

		// Packing type (always 0)
		fgetc(in);

		// Number of rows in pattern (1...256)
		fread(data, 1, 2, in);
		rows = _convert16(data);
//		printf("rows: %d\n", rows);

		// Packed patterndata size
		fread(data, 1, 2, in);
		size =	_convert16(data);
//		printf("size: %d\n", size);
		
		if (size == 0) {
			free(data);
			return;
		}
		data2 = (unsigned char*) malloc(size);
		oData = (unsigned char*) malloc(size);
		fread(data2, 1, size, in);
//		fputc(rows&0xff, out);

		for (i = 0; i < rows; i++) {
			int channel;
			int rowInfo = 0;

			int chn1 = -1;
			int chn3 = -1;

			for (channel = 0; channel < 4; channel++) {
				int check = data2[patternpos++];
				int currentNote = -1;

				if ((check & 0x80) != 0) {
					// note
					if ((check & 0x1) != 0)	currentNote = data2[patternpos++];

					// instrument
					if ((check & 0x2) != 0) patternpos++;
					// volume
					if ((check & 0x4) != 0) patternpos++;
					// effect
					if ((check & 0x8) != 0) patternpos++;
					// effect param
					if ((check & 0x10) != 0) patternpos++;
				} else {
					currentNote	= check;
					patternpos++;
					patternpos++;
					patternpos++;
					patternpos++;
				}

//				if (currentNote == -1) printf("--- ");
//				else if (channel != 1) printf("%.3s ", notes[currentNote]);
//				else printf("%.3d ", currentNote);
//				if (channel == 1) printf("%d\n", currentNote);

				if (currentNote == -1) continue;
				else {
					rowInfo |= 1 << (7-channel);
					if (channel == 0 && currentNote != 97) {
						rowInfo |= 2;
					} else if (channel == 2 && currentNote != 97) {
						rowInfo |= 1;
					} else if (channel == 1) {
						if (currentNote == 97) rowInfo |= 4;
						else if (currentNote == 49) rowInfo |= 8;
						else chn1 = currentNote;
					} else if (channel == 3) {
						chn3 = currentNote;
					}
				}
			}
//			printf("\n");
			oData[oDataSize++] = rowInfo;
//			fputc(rowInfo, out);
			if (chn1 != -1) oData[oDataSize++] = chn1;
			if (chn3 != -1) oData[oDataSize++] = chn3;
		}

		fputc(oDataSize, out);
		fwrite(oData, 1, oDataSize, out);

		free(data2);
		free(oData);
}

void xm_parse(FILE *in, FILE *out) {
	uint16 songLength = 0;
	uint16 patternLength = 0;

	int i;
	int test;
	int	patData;

	// "Extended Module: "
	unsigned char *data = (unsigned char*) malloc(512);
	fread(data, 1, 17, in);

	// Module name
	fread(data, 1, 20, in);

	fgetc(in);

	// Tracker name, version, header size
	fread(data, 1, 20 + 2 + 4, in);

	// Song length (in pattern order table)
	fread(data, 1, 2, in);
//	fputc(data[1], out);
//	fwrite(data, 1, 2, out);
	songLength = _convert16(data);

	// Restart position
	fread(data, 1, 2, in);

	// Number of channels (2,4,6,8,10,...,32)
	fread(data, 1, 2, in);

	// Number of patterns (max 128)
	fread(data, 1, 2, in);
	patternLength = _convert16(data);

	fputc(songLength, out);
	fputc(patternLength, out);

	// Number of instruments (max 128)
	fread(data, 1, 2, in);

	// Flags: bit 0: 0 = Amiga frequency table;
	//               1 = Linear frequency table
	// tempo, bpm
	fread(data, 1, 2 + 2 + 2, in);
	fread(data, 1, 256, in);

	for (i = 0, test = 0, patData = 0; i < songLength; i++) {
		patData <<= 4;
		patData |= data[i]&0x0F;

		if (++test == 2) {
			fputc(patData, out);
			test = 0;
			patData = 0;
		}
	}
	printf("\n");

	for (i = 0; i < patternLength; i++) {
		_xm_convertPattern(in, out);
	}

	free(data);
}