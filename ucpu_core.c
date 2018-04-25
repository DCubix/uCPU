#include "ucpu_core.h"
#include "ucpu_ops.h"

uCPU* ucpu_new(u8* program, u16 size) {
	uCPU* cpu = (uCPU*) malloc(sizeof(uCPU));
	cpu->call_stack = ustack_new();
	cpu->stack = ustack_new();
	cpu->ram = umem_new(0x8000);
	cpu->pc = 0;
	cpu->ticks = 0;
	cpu->stop = false;
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
	u8 i = ucpu_fetch(cpu);
	if (uops_op_exists(i)) {
		uCPU_Ops[i].fn(cpu);
	} else {
		LOG("Unknown instruction.");
	}
}

u8 ucpu_fetch(uCPU* cpu) {
	return umem_read(cpu->ram, cpu->pc++);
}

u16 ucpu_fetch16(uCPU* cpu) {
	u16 v = umem_read16(cpu->ram, cpu->pc++);
	cpu->pc++;
	return v;
}

void ucpu_run(uCPU* cpu) {
	cpu->ticks = 0;
	cpu->pc = 0;
	while (!cpu->stop && cpu->ticks < UCPU_MAX_TICKS) {
		ucpu_tick(cpu);
		cpu->ticks++;
	}
}