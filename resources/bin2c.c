#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#if defined(_WIN32)
#pragma warning(disable : 4996) // The POSIX name for this item is deprecated.
#endif

int main(int argc, char **argv) {
	FILE *f_c, *f_h, *f_i;
	unsigned int i, j, count, size;
	unsigned char buf[12];

	if (argc < 3) {
		fprintf(stderr, "Syntax: bin2c <C file> <H file> [Input file 1] [Input file 2] ..\n");
		return -1;
	}
	f_c = fopen(argv[1], "w");
	if (!f_c) {
		fprintf(stderr, "Error opening C file: %s: %d %s\n", argv[1], errno, strerror(errno));
		return -1;
	}
	f_h = fopen(argv[2], "w");
	if (!f_h) {
		fprintf(stderr, "Error opening H file: %s: %d %s\n", argv[2], errno, strerror(errno));
		return -1;
	}

	fprintf(f_c, "/* Autogenerated by bin2c */\n\n");
	fprintf(f_c, "#include <stdint.h>\n\n");
	fprintf(f_h, "#if !defined(__RESOURCES_H)\n");
	fprintf(f_h, "#define __RESOURCES_H\n\n");
	fprintf(f_h, "/* Autogenerated by bin2c */\n\n");
	fprintf(f_h, "#include <stdint.h>\n\n");

	for (i=3; i<(unsigned int) argc; ++i) {
		char *name = strdup(strrchr(argv[i], '/') + 1);
		for (j=0; j<strlen(name); ++j) {
			if (name[j] == '.' || name[j] == '-')
				name[j] = '_';
			else
				name[j] = tolower(name[j]);
		}

		f_i = fopen(argv[i], "rb");
		if (!f_i) {
			fprintf(stderr, "Error opening data file %s: %d %s\n", argv[i], errno, strerror(errno));
			return -1;
		}

		fprintf(f_c, "uint8_t %s[] = {\n", name);
		fprintf(f_h, "extern uint8_t %s[];\n", name);

		size = 0;
		while ((count = (unsigned int) fread(buf, 1, sizeof(buf), f_i)) > 0) {
			fprintf(f_c, "\t");
			for (j=0; j<count; j++) {
				fprintf(f_c, "0x%02x, ", (unsigned int) buf[j]);
				size++;
			}
			fprintf(f_c, "\n");
		}
		fprintf(f_c, "\t0x00\n");

		fprintf(f_c, "};\n\n");
		fprintf(f_h, "extern uint32_t %s_size;\n\n", name);
		fprintf(f_c, "uint32_t %s_size = %i;\n\n", name, size);
		fclose(f_i);
		free(name);
	}
	fprintf(f_h, "#endif /* __RESOURCES_H */\n");

	fclose(f_c);
	fclose(f_h);

	return 0;
}
