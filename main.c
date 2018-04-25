#include <stdio.h>
#include <stdlib.h>

#include "ucpu.h"

int main(int argc, char** argv) {
	u16 program[] = {
		UOP("dq"), 4, 1, 1, 4,
		UOP("dq"), 1, 0, 1, 1,
		UOP("dq"), 1, 1, 1, 1,
		UOP("dq"), 4, 1, 1, 4,
		UOP("cls"), 0x3, aLit,
		UOP("mov"), R0, aReg, 0x2, aLit,
		UOP("inc"), R1, aReg,
		UOP("inc"), R2, aReg,
		UOP("mov"), R3, aReg, 0x0, aLit,
		UOP("mul"), R3, aReg, R2, aReg, 120, aLit,
		UOP("add"), R3, aReg, R3, aReg, R1, aReg,
		UOP("vmov"), R3, aReg, R0, aReg,
		UOP("jlt"), R1, aReg, 96, aLit, 28, aLit,
		UOP("vdrw"), 64, aLit, 32, aLit, 0x4000, aLit,
		UOP("flip"),
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

