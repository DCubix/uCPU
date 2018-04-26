#include <stdio.h>
#include <stdlib.h>

#include "ucpu.h"

int main(int argc, char** argv) {

	char prog[] = 
	"start:\n"
	"	mov $2 2\n"
	"	psh $2\n"
	"	sys 0xA001\n"
	"	call line\n"
	"	sys 0xA002\n"
	"	end\n"
	"line:\n"
	"	add $0 1\n"
	"	mov $1 $0\n"
	"	mul $1 120\n"
	"	add $1 $0\n"
	"	vmov $1 0\n"
	"	cmp $0 32\n"
	"	jne line\n"
	"	ret";

	u16 program_size;
	u16 *program = uasm_transform(prog, &program_size);
	
	uCPU* cpu = ucpu_new(program, program_size);
	
	FILE* fp = fopen("dump.bin", "wb");
	if (fp) {
		umem_dump(cpu->ram, fp);
		fclose(fp);
	}
	
	ucpu_run(cpu);
	
	ucpu_free(cpu);
	return (EXIT_SUCCESS);
}

