#include "ucpu_core.h"
#include "ucpu_ops.h"

uCPU* ucpu_new(u8* program, u16 size) {
	uCPU* cpu = (uCPU*) malloc(sizeof(uCPU));
	cpu->call_stack = ustack_new();
	cpu->stack = ustack_new();
	cpu->ram = umem_new(0x4000); // 16kb of data ram. 8Kb of data (0x0000 to 0x1FFF)
	cpu->program = umem_new(0x4000); // 16kb of program memory
	cpu->gfx = ugfx_new();
	cpu->pc = 0;
	cpu->ticks = 0;
	cpu->stop = false;
	if (program != NULL && size > 0) {
		for (u16 i = 0; i < size; i++) umem_write(cpu->program, i, program[i]);
	}
	return cpu;
}

void ucpu_free(uCPU* cpu) {
	umem_free(cpu->ram);
	umem_free(cpu->program);
	free(cpu->stack);
	free(cpu->call_stack);
	free(cpu);
}

void ucpu_tick(uCPU* cpu) {
	u8 i = ucpu_fetch(cpu);
	if (uops_op_exists(i)) {
		uCPU_Ops[i].fn(cpu);
	} else {
		LOG("Unknown instruction.");
	}
}

u8 ucpu_fetch(uCPU* cpu) {
	return umem_read(cpu->program, cpu->pc++);
}

u16 ucpu_fetch16(uCPU* cpu) {
	u16 v = umem_read16(cpu->program, cpu->pc++);
	cpu->pc++;
	return v;
}

u8* ucpu_fetch_auto(uCPU* cpu, u8 reqtype, u8* out) {
	u8 val = ucpu_fetch(cpu);
	u8 type = ucpu_fetch(cpu);
	
	if (reqtype == uArgType_Any) {
retval:
		if (type == uArgType_Register) {
			if (out != NULL) *out = cpu->reg[val];
			return &cpu->reg[val];
		} else if (type == uArgType_Memory) {
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
		SDL_Delay(16);

		cpu->ticks++;
	}
	
	ugfx_free(cpu->gfx);
}