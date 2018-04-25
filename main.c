#include <stdio.h>
#include <stdlib.h>

#include "ucpu.h"

int main(int argc, char** argv) {
	const u8 program[] = {
		UOP("put"), 'H', uArgType_Literal,
		UOP("put"), 'e', uArgType_Literal,
		UOP("put"), 'l', uArgType_Literal,
		UOP("put"), 'l', uArgType_Literal,
		UOP("put"), 'o', uArgType_Literal,
		UOP("put"), '!', uArgType_Literal,
		UOP("end")
	};
	uCPU* cpu = ucpu_new(program, LEN(program));
	
	FILE* fp = fopen("dump.bin", "wb");
	if (fp) {
		umem_dump(cpu->ram, fp);
		fclose(fp);
	}
	
	ucpu_run(cpu);
	
	ucpu_free(cpu);
	return (EXIT_SUCCESS);
}

