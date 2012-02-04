#include <stdio.h>

#include "xm.h"

int main(int argc, char **argv) {
	FILE *in;
	FILE *out;

	printf(
		"quarn/outbreaks 4k-softsynth fileconvertor (airwolf4k)\n"
		"---------------------------------------------------\n"
	);

	if (argc != 3) {
		printf(
			"usage: \n"
			"%s <infile.xm> <outfile.xm>\n",
			argv[0]
		);
		return 1;
	}

	in = fopen(argv[1], "rb");
	if (in == NULL) {
		fprintf(stderr, "file \"%s\" could not be opened for reading.\n", argv[1]);
		return 1;
	}

	out = fopen(argv[2], "wb");
	if (out == NULL) {
		fprintf(stderr, "file \"%s\" could not be opened for writing.\n", argv[1]);
		fclose(in);
		return 1;
	}
	printf("InputFile: %s\nOutputFile: %s\n", argv[1], argv[2]);

	xm_parse(in, out);

	fclose(in);
	fclose(out);

	return 0;
}
