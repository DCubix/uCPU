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
#ifndef DEBUG
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
		} else if (strend(argv[1], ".asm") || strend(argv[1], ".ASM") || strend(argv[1], ".s") || strend(argv[1], ".S")) {
			fp = fopen(argv[1], "r");
			if (!fp) {
				printf("Invalid file.\n");
				return EXIT_FAILURE;
			}

			program = uasm_transform(fp, &program_size);
		}
		
		if (fp) {
			fclose(fp);
	
			uCPU* cpu = ucpu_new(program, program_size);

			FILE* dfp = fopen("dump.bin", "wb");
			if (dfp) {
				umem_dump(cpu->ram, dfp);
				fclose(dfp);
			}

			ucpu_run(cpu);

			ucpu_free(cpu);
		}
	}
#else
//	char prog[] = 
//	"mov $0 100 ; Move 100 to register 0\n"
//	"mul $0 2 ; Multiply register 0 by 2\n"
//	"; Divide register 0 by 3\n"
//	"div $0 3\n"
//	"outn $0\n"
//	"out 10\n"
//	"end";
//	
//	u16* program = program = uasm_transform(prog, &program_size);
//	uCPU* cpu = ucpu_new(program, program_size);
//
//	FILE* dfp = fopen("dump.bin", "wb");
//	if (dfp) {
//		umem_dump(cpu->ram, dfp);
//		fclose(dfp);
//	}
//
//	ucpu_run(cpu);
//
//	ucpu_free(cpu);
	
	FILE* fp = fopen("test.ASM", "r");
	if (!fp) {
		printf("Invalid file.\n");
		return EXIT_FAILURE;
	}

	u16 program_size;
	u16* program;
	program = uasm_transform(fp, &program_size);
	uCPU* cpu = ucpu_new(program, program_size);

	FILE* dfp = fopen("dump.bin", "wb");
	if (dfp) {
		umem_dump(cpu->ram, dfp);
		fclose(dfp);
	}

	ucpu_run(cpu);

	ucpu_free(cpu);
#endif
	return EXIT_SUCCESS;
}

