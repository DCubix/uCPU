#include <stdio.h>
#include <stdlib.h>

#include "ucpu.h"

int strend(const char *s, const char *t) {
    size_t ls = strlen(s); // find length of s
    size_t lt = strlen(t); // find length of t
    if (ls >= lt) { // check if t can fit in s
        // point s to where t should start and compare the strings from there
        return (0 == memcmp(t, s + (ls - lt), lt));
    }
    return 0; // t was longer than s
}

int main(int argc, char** argv) {
	u16 program_size;
	u16 *program;
	if (argc > 1) {
		FILE* fp = NULL;
		if (strend(argv[1], ".bin")) {
			fp = fopen(argv[1], "rb");
			if (!fp) {
				printf("Invalid file.\n");
				return EXIT_FAILURE;
			}

			fseek(fp, 0, SEEK_END);
			program_size = ftell(fp) / 2;
			rewind(fp);
			
			program = (u16*) malloc(sizeof(u16) * program_size);
			fread(program, sizeof(u16), program_size, fp);
		} else if (strend(argv[1], ".asm") || strend(argv[1], ".s") || strend(argv[1], ".S")) {
			fp = fopen(argv[1], "r");
			if (!fp) {
				printf("Invalid file.\n");
				return EXIT_FAILURE;
			}
			fseek(fp, 0, SEEK_END);
			u16 sz = ftell(fp);
			rewind(fp);
			
			char* prog = (char*) malloc(sizeof(char) * sz);
			fread(prog, sizeof(char), sz, fp);
			prog[sz] = 0;
			program = uasm_transform(prog, &program_size);
		}
		
		if (fp) fclose(fp);
	
		uCPU* cpu = ucpu_new(program, program_size);

		FILE* dfp = fopen("dump.bin", "wb");
		if (dfp) {
			umem_dump(cpu->ram, dfp);
			fclose(dfp);
		}

		ucpu_run(cpu);

		ucpu_free(cpu);

		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

