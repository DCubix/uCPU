#include <stdio.h>
#include <stdlib.h>

#include "ucpu_core.h"

int main(int argc, char** argv) {
	
	const u8 program[] = {
		0x1, 'H', uArgType_Literal,
		0x1, 'e', uArgType_Literal,
		0x1, 'l', uArgType_Literal,
		0x1, 'l', uArgType_Literal,
		0x1, 'o', uArgType_Literal,
		0x1, '!', uArgType_Literal,
		0x0
	};
	uCPU* cpu = ucpu_new(program, LEN(program));
	
	ucpu_run(cpu);
	
	ucpu_free(cpu);
	return (EXIT_SUCCESS);
}

