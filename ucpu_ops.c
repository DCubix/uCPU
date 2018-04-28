#include "ucpu_ops.h"

UCPU_DEFINE_OP(nop) {}
UCPU_DEFINE_OP(end) { cpu->stop = true; }

UCPU_DEFINE_OP(in) { // IN reg/mem
	u16 dst = ucpu_fetch(cpu);
	u16 val = 0;
	scanf("%hu", &val);
	
	switch (form) {
		case uArg_R: cpu->reg[dst] = val; break;
		case uArg_M: umem_write(cpu->ram, dst, val); break;
	}
}

UCPU_DEFINE_OP(out) { // OUT any
	u16 src = ucpu_fetch(cpu);
	u16 val = 0;
	switch (form) {
		case uArg_I: val = src; break;
		case uArg_R: val = cpu->reg[src]; break;
		case uArg_M: val = umem_read(cpu->ram, src); break;
	}
	putchar(val);
}

UCPU_DEFINE_OP(outn) { // OUTn any
	u16 src = ucpu_fetch(cpu);
	u16 val = 0;
	switch (form) {
		case uArg_I: val = src; break;
		case uArg_R: val = cpu->reg[src]; break;
		case uArg_M: val = umem_read(cpu->ram, src); break;
	}
	printf("%hu", val);
}

UCPU_DEFINE_OP(psh) { // PSH r0
	u16 src = ucpu_fetch(cpu);
	u16 val = 0;
	switch (form) {
		case uArg_I: val = src; break;
		case uArg_R: val = cpu->reg[src]; break;
		case uArg_M: val = umem_read(cpu->ram, src); break;
	}
	ustack_push(cpu->stack, val);
}

UCPU_DEFINE_OP(pop) { // POP r0
	u16 dstR = ucpu_fetch(cpu);
	cpu->reg[dstR] = ustack_pop(cpu->stack);
}

UCPU_DEFINE_OP(mov) { // MOV r0/mem r0/mem/i
	u16 dst = ucpu_fetch(cpu);
	u16 src = ucpu_fetch(cpu);
	
	switch (form) {
		case uArg_RI: cpu->reg[dst] = src; break;
		case uArg_RR: cpu->reg[dst] = cpu->reg[src]; break;
		case uArg_RM: cpu->reg[dst] = umem_read(cpu->ram, src); break;
		case uArg_MI: umem_write(cpu->ram, dst, src); break;
		case uArg_MR: umem_write(cpu->ram, dst, cpu->reg[src]); break;
		case uArg_MM: umem_write(cpu->ram, dst, umem_read(cpu->ram, src)); break;
		default:
			printf("MOV: Unknown parameters!\n");
			break;
	}
}

UCPU_DEFINE_OP(vmov) { // VMOV mem r0/ram/i (for video memory)
	u16 dst = ucpu_fetch(cpu);
	u16 src = ucpu_fetch(cpu);
	
	switch (form) {
		case uArg_RI: umem_write(cpu->gfx->vram, cpu->reg[dst], src); break;
		case uArg_RR: umem_write(cpu->gfx->vram, cpu->reg[dst], cpu->reg[src]); break;
		case uArg_RM: umem_write(cpu->gfx->vram, cpu->reg[dst], umem_read(cpu->ram, src)); break;
		case uArg_MI: umem_write(cpu->gfx->vram, dst, src); break;
		case uArg_MR: umem_write(cpu->gfx->vram, dst, cpu->reg[src]); break;
		case uArg_MM: umem_write(cpu->gfx->vram, dst, umem_read(cpu->ram, src)); break;
		default:
			printf("VMOV: Unknown parameters!\n");
			break;
	}
}

UCPU_DEFINE_OP(vpix) { // VPIX color (pops 2 values from the stack (x and y))
	u16 col = ucpu_fetch(cpu);
	u16 x = ustack_pop(cpu->stack);
	u16 y = ustack_pop(cpu->stack);
	
	ugfx_set(cpu->gfx, x, y, col);
}

UCPU_DEFINE_OP(vspr) { // VSPR memloc/reg (pops 2 values from the stack (x and y))
	u16 loc = ucpu_fetch(cpu);
	u16 x = ustack_pop(cpu->stack);
	u16 y = ustack_pop(cpu->stack);
	
	u16 src = 0;
	switch (form) {
		case uArg_M: src = loc; break;
		case uArg_R: src = cpu->reg[loc]; break;
		default:
			printf("Invalid argument.\n");
			return;
	}
	
	u16 addr = src;
	for (u16 py = 0; py < 8; py++) {
		for (u16 px = 0; px < 8; px++) {
			ugfx_set(cpu->gfx, px+x, py+y, umem_read(cpu->ram, addr));
			addr++;
		}
	}
}

UCPU_DEFINE_OP(sys) {

	u16 c = ucpu_fetch(cpu);
	switch (c) {
		case uSys_Reset: {
			cpu->pc = 0;
			cpu->stack->top = -1;
			memset(cpu->ram->data, 0, sizeof(u16) * cpu->ram->size);
			memset(cpu->reg, 0, sizeof(u16) * RCount);
		} break;
		case uSys_Gfx_Clear: {
			u8 color = 0;
			if (!ustack_empty(cpu->stack)) color = ustack_pop(cpu->stack);
			ugfx_clear(cpu->gfx, color);
		} break;
		case uSys_Gfx_Flip: {
			ugfx_flip(cpu->gfx);
		} break;
	}
}

UCPU_DEFINE_OP(cmp) { // CMP r0 r1
	u16 a = ucpu_fetch(cpu);
	u16 b = ucpu_fetch(cpu);
	switch (form) {
		case uArg_RI: a = cpu->reg[a]; break;
		case uArg_RR: a = cpu->reg[a]; b = cpu->reg[b]; break;
		case uArg_RM: a = cpu->reg[a]; b = umem_read(cpu->ram, b); break;
		case uArg_MI: a = umem_read(cpu->ram, a); break;
		case uArg_MR: a = umem_read(cpu->ram, a); b = cpu->reg[b]; break;
		case uArg_MM: a = umem_read(cpu->ram, a); b = umem_read(cpu->ram, b); break;
		default:
			printf("CMP: Unknown parameters!\n");
			break;
	}
	
	i16 v = a - b;
	if		(v == 0) cpu->zero = 1, cpu->carry = 0;
	else if (v >  0) cpu->zero = 0, cpu->carry = 0;
	else if (v <  0) cpu->zero = 0, cpu->carry = 1;
}

// Jxx loc
#define UCPU_DEFINE_COND(name, chk) \
UCPU_DEFINE_OP(name) { \
	u16 loc = ucpu_fetch(cpu); \
	bool z = cpu->zero, c = cpu->carry; \
	if (chk) { \
		cpu->pc = loc; \
	} \
}

UCPU_DEFINE_OP(jmp) {
	u16 loc = ucpu_fetch(cpu);
	cpu->pc = loc;
}

UCPU_DEFINE_COND(jeq, (z && !c))
UCPU_DEFINE_COND(jne, !(z && !c))
UCPU_DEFINE_COND(jlt, (!z && c))
UCPU_DEFINE_COND(jgt, (!z && !c))
UCPU_DEFINE_COND(jle, (!z && c) || (z && !c))
UCPU_DEFINE_COND(jge, (!z && !c) || (z && !c))

		
// OP a b (OP the value of b to a)
#define UCPU_DEFINE_BOP(name, op) \
UCPU_DEFINE_OP(name) { \
	u16 dst = ucpu_fetch(cpu); \
	u16 src = ucpu_fetch(cpu); \
	switch (form) { \
		case uArg_RI: cpu->reg[dst] = cpu->reg[dst] op src; break; \
		case uArg_RR: cpu->reg[dst] = cpu->reg[dst] op cpu->reg[src]; break; \
		case uArg_RM: cpu->reg[dst] = cpu->reg[dst] op umem_read(cpu->ram, src); break; \
		case uArg_MI: cpu->ram->data[dst] = cpu->ram->data[dst] op src; break; \
		case uArg_MR: cpu->ram->data[dst] = cpu->ram->data[dst] op cpu->reg[src]; break; \
		case uArg_MM: cpu->ram->data[dst] = cpu->ram->data[dst] op umem_read(cpu->ram, src); break; \
		default: \
			printf("BINOP: Unknown parameters!\n"); \
			break; \
	} \
}

UCPU_DEFINE_BOP(add, +);
UCPU_DEFINE_BOP(sub, -);
UCPU_DEFINE_BOP(mul, *);
UCPU_DEFINE_BOP(div, /);
UCPU_DEFINE_BOP(mod, %);
UCPU_DEFINE_BOP(shl, <<);
UCPU_DEFINE_BOP(shr, >>);
UCPU_DEFINE_BOP(or, |);
UCPU_DEFINE_BOP(and, &);
UCPU_DEFINE_BOP(xor, ^);

UCPU_DEFINE_OP(not) {
	u16 dst = ucpu_fetch(cpu);
	switch (form) {
		case uArg_R: cpu->reg[dst] = ~cpu->reg[dst]; break;
		case uArg_M: umem_write(cpu->ram, dst, ~umem_read(cpu->ram, dst)); break;
		default:
			printf("NOT: Unknown parameters!\n");
			break;
	}
}

UCPU_DEFINE_OP(call) {
	u16 loc = ucpu_fetch(cpu);
	ustack_push(cpu->call_stack, cpu->pc);
	cpu->pc = loc;
}

UCPU_DEFINE_OP(ret) {
	if (ustack_empty(cpu->call_stack)) {
		printf("RET: Cannot return!\n");
		return;
	}
	cpu->pc = ustack_pop(cpu->call_stack);
}

const uOp uCPU_Ops[] = {
	{ "nop", op(nop) },
	{ "end", op(end) },
	{ "in", op(in) },
	{ "out", op(out) },
	{ "outn", op(outn) },
	{ "sys", op(sys) },
	{ "psh", op(psh) },
	{ "pop", op(pop) },
	{ "mov", op(mov) },
	{ "vmov", op(vmov) },
	{ "vpix", op(vpix) },
	{ "vspr", op(vspr) },
	{ "cmp", op(cmp) },
	{ "jmp", op(jmp) },
	{ "jeq", op(jeq) },
	{ "jne", op(jne) },
	{ "jlt", op(jlt) },
	{ "jgt", op(jgt) },
	{ "jle", op(jle) },
	{ "jge", op(jge) },
	{ "add", op(add) },
	{ "sub", op(sub) },
	{ "mul", op(mul) },
	{ "div", op(div) },
	{ "mod", op(mod) },
	{ "shl", op(shl) },
	{ "shr", op(shr) },
	{ "and", op(and) },
	{ "or", op(or) },
	{ "xor", op(xor) },
	{ "not", op(not) },
	{ "call", op(call) },
	{ "ret", op(ret) },
	{ NULL, NULL } // guard
};

u16 uops_get_op(const char* name) {
	u16 i = 0;
	while (uCPU_Ops[i].name != NULL) {
		if (strcasecmp(name, uCPU_Ops[i].name) == 0) return i;
		i++;
	}
	return 0;
}

const char* uops_get_op_name(u16 id) {
	u16 i = 0;
	while (uCPU_Ops[i].name != NULL) {
		if (i == id) return uCPU_Ops[i].name;
		i++;
	}
	return NULL;
}

bool uops_op_exists(u16 index) {
	u16 i = 0;
	while (uCPU_Ops[i].name != NULL) {
		if (i == index) return true;
		i++;
	}
	return false;
}