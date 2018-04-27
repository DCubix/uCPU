#include "ucpu_core.h"
#include "ucpu_ops.h"

#include <string.h>

uCPU* ucpu_new(u16* program, u16 size) {
	uCPU* cpu = (uCPU*) malloc(sizeof(uCPU));
	cpu->call_stack = ustack_new();
	cpu->stack = ustack_new();
	cpu->ram = umem_new(0x6000); // 48kb of memory, 32kb of program (0x0000 - 0x3FFF), 16kb of data (0x4000 - 0x5FFF)
	cpu->gfx = ugfx_new();
	cpu->pc = 0;
	cpu->ticks = 0;
	cpu->stop = false;
	cpu->zero = false;
	cpu->carry = false;
	memset(cpu->reg, 0, RCount * sizeof(u16));
	if (program != NULL && size > 0) {
		for (u16 i = 0; i < size; i++) umem_write(cpu->ram, i, program[i]);
	}
	return cpu;
}

void ucpu_free(uCPU* cpu) {
	umem_free(cpu->ram);
	free(cpu->stack);
	free(cpu->call_stack);
	free(cpu);
}

void ucpu_tick(uCPU* cpu) {
	u16 i = ucpu_fetch(cpu);
	u8 instr = (i & 0xFF00) >> 8;
	u8 form = (i & 0x00FF);
	if (uops_op_exists(instr)) {
		uCPU_Ops[instr].fn(cpu, form);
	} else {
		printf("Unknown instruction: 0x%hhx\n", instr);
	}
}

u16 ucpu_fetch(uCPU* cpu) {
	return umem_read(cpu->ram, cpu->pc++);
}

void ucpu_run(uCPU* cpu) {
	cpu->ticks = 0;
	cpu->pc = 0;
	
	SDL_Event e;
	
	bool running = true;
	while (running) {
		if (cpu->stop || cpu->ticks >= UCPU_MAX_TICKS) {
			running = false;
		}
		
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
		}
		
		ucpu_tick(cpu);
//		printf("[");
//		for (u16 i = 0; i < RCount; i++) {
//			printf("%5hu ", cpu->reg[i]);
//		}
//		printf("]\n");
//		SDL_Delay(1);
		
		cpu->ticks++;
	}
	SDL_Delay(100);
	ugfx_flip(cpu->gfx);
	ugfx_save_screen(cpu->gfx, "snap.bmp");
//	SDL_Delay(2000);

	ugfx_free(cpu->gfx);
}