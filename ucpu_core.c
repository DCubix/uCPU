#include "ucpu_core.h"
#include "ucpu_ops.h"

uCPU* ucpu_new(u16* program, u16 size) {
	uCPU* cpu = (uCPU*) malloc(sizeof(uCPU));
	cpu->call_stack = ustack_new();
	cpu->stack = ustack_new();
	cpu->ram = umem_new(0x6000); // 24kb of memory, 16kb of program (0x0000 - 0x3FFF), 8kb of data (0x4000 - 0x5FFF)
	cpu->gfx = ugfx_new();
	cpu->pc = 0;
	cpu->ticks = 0;
	cpu->stop = false;
	cpu->dbptr = 0x4000;
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
	if (uops_op_exists(i)) {
		uCPU_Ops[i].fn(cpu);
	} else {
		LOG("Unknown instruction.");
	}
}

u16 ucpu_fetch(uCPU* cpu) {
	return umem_read(cpu->ram, cpu->pc++);
}

u16* ucpu_fetch_auto(uCPU* cpu, u8 reqtype, u16* out) {
	u16 val = ucpu_fetch(cpu);
	u16 type = ucpu_fetch(cpu);
	
	if (reqtype == aAny) {
retval:
		if (type == aReg) {
			if (out != NULL) *out = cpu->reg[val];
			return &cpu->reg[val];
		} else if (type == aMem) {
			if (out != NULL) *out = cpu->ram->data[val];
			return &cpu->ram->data[val];
		} else {
			if (out != NULL) *out = val;
			return NULL;
		}
	} else {
		if ((reqtype & type) == type) {
			goto retval;
		}
	}
	
	LOG("Invalid argument.");
	return NULL;
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

		cpu->ticks++;
	}
	SDL_Delay(5000);

	ugfx_free(cpu->gfx);
}