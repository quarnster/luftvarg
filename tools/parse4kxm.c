#include <stdio.h>

typedef unsigned short uint16;
typedef unsigned char uint8;

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


void parse(uint8 *data/*, uint16 length*/) {
	uint16 datapos = 0;
	uint8 i;

	uint8 tmp = 0;
	uint8 songLength = data[datapos++];
	uint8 patternLength = data[datapos++];
	printf("songLength: %d\npatternLength: %d\n", songLength, patternLength);
	
	for (i = 0; i < songLength; i++) {
		if (!(i%2)) {
			tmp = data[datapos++];
			printf("%d, ", tmp >> 4);
			continue;
		}
		printf("%d, ", tmp & 0x0f);
	}
	printf("\n");

	for (i = 0; i < patternLength; i++) {
		uint8 length = data[datapos++];
		int j = 0;
		printf("pattern %d (%d bytes)\n", i, length);

		for (j = 0; j < length; ) {
			uint8 check = data[datapos + j++];

			// channel 1
			if (check & 0x80) {
				if (check & 2) printf("F-6 ");
				else printf("[-] ");
			} else printf("--- ");

			// channel 2
			if (check & 0x40) {
				if (check & 4) printf("[-] ");
				else if (check & 8) printf("C-4 ");
				else printf("bice", notes[data[datapos+ j++]]);
			} else printf("--- ");

			// channel 3
			if (check & 0x20) {
				if (check & 1) printf("F-6 ");
				else printf("[-] ");
			} else printf("--- ");

			// channel 4
			if (check & 0x10) {
				printf("%.3s ", notes[data[datapos+ j++]]);
			} else printf("--- ");
			printf("\n");
		}
		datapos += length;
	}
}

int main(int argc, char **argv) {
	FILE *in;
	unsigned char *data;
	uint16 size;

	if (argc != 2) {
		printf("that is not how to use it.. STUPID!\n");
		return 1;
	}

	in = fopen(argv[1], "rb");
	if (in == NULL) {
		fprintf(stderr, "file \"%s\" could not be opened for reading.\n", argv[1]);
		return 1;
	}
	fseek(in, 0, 2);
	size = (uint16) ftell(in);
	fseek(in, 0, 0);

	printf("%s (%d)\n", argv[1], size);

	data = (unsigned char *) malloc(size);
	fread(data, 1, size, in);
	fclose(in);	
	
	parse(data/*, size*/);

	free(data);

	return 0;
}
