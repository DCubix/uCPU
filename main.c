#include <stdio.h>
#include <stdlib.h>

#include "ucpu.h"

int main(int argc, char** argv) {
	u8 program[] = {
		UOP("cls"), 0x3, uArgType_Literal,
		UOP("vpok"), 10, uArgType_Literal, 10, uArgType_Literal, 0x2, uArgType_Literal,
		UOP("vpok"), 20, uArgType_Literal, 20, uArgType_Literal, 0x2, uArgType_Literal,
		UOP("vpok"), 30, uArgType_Literal, 30, uArgType_Literal, 0x2, uArgType_Literal,
		UOP("vpok"), 40, uArgType_Literal, 40, uArgType_Literal, 0x2, uArgType_Literal,
		UOP("flip"),
		UOP("end")
	};
	uCPU* cpu = ucpu_new(program, LEN(program));
	
	FILE* fp = fopen("dump.bin", "wb");
	if (fp) {
		umem_dump(cpu->program, fp);
		fclose(fp);
	}
	
	ucpu_run(cpu);
	
	ucpu_free(cpu);
	return (EXIT_SUCCESS);
}

