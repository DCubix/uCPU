#include "ucpu_ops.h"

UCPU_DEFINE_OP(end) {
	cpu->stop = true;
}

UCPU_DEFINE_OP(put) {
	u8 v = ucpu_fetch(cpu);
	u8 type = ucpu_fetch(cpu);
	
	u8 ch = IS(type, uArgType_Literal) ? v : umem_read(cpu->ram, v);
	putc(ch, stdout);
}

UCPU_DEFINE_OP(mov) {
	// mov dst src
	// mov [0x34] 10
	// mov [0x23] [0x56]
	u8 v0 = ucpu_fetch(cpu);
	u8 t0 = ucpu_fetch(cpu);
	u8 v1 = ucpu_fetch(cpu);
	u8 t1 = ucpu_fetch(cpu);
	
	if (!IS(t0, uArgType_Memory)) {
		LOG("First argument should be a memory location!");
		return;
	}
	
	u8 v = IS(t1, uArgType_Literal) ? v1 : umem_read(cpu->ram, v1);	
	umem_write(cpu->ram, v0, v);
}

UCPU_DEFINE_OP(jmp) {
	u8 v0 = ucpu_fetch(cpu);
	u8 t0 = ucpu_fetch(cpu);
	
	if (!IS(t0, uArgType_Literal)) {
		LOG("First argument should be a literal!");
		return;
	}
	
	cpu->pc = v0;
}

#define UCPU_DEFINE_COND_JMP(name, op) \
UCPU_DEFINE_OP(name) { \
	u8 va = ucpu_fetch(cpu); \
	u8 at = ucpu_fetch(cpu); \
	u8 vb = ucpu_fetch(cpu); \
	u8 bt = ucpu_fetch(cpu); \
	u8 loc = ucpu_fetch(cpu); \
	u8 loct = ucpu_fetch(cpu); \
	if (!IS(loct, uArgType_Literal)) { \
		LOG("Last argument should be a literal!"); \
		return; \
	} \
	u8 a = IS(at, uArgType_Literal) ? va : umem_read(cpu->ram, va); \
	u8 b = IS(bt, uArgType_Literal) ? vb : umem_read(cpu->ram, vb); \
	if (a op b) cpu->pc = loc; \
}

UCPU_DEFINE_COND_JMP(jeq, ==)
UCPU_DEFINE_COND_JMP(jne, !=)
UCPU_DEFINE_COND_JMP(jlt, <)
UCPU_DEFINE_COND_JMP(jgt, >)
UCPU_DEFINE_COND_JMP(jle, <=)
UCPU_DEFINE_COND_JMP(jge, >=)

UCPU_DEFINE_OP(push) {
	u8 v = ucpu_fetch(cpu);
	u8 t = ucpu_fetch(cpu);
	
	u8 val = IS(t, uArgType_Literal) ? v : umem_read(cpu->ram, v);
	ustack_push(cpu->stack, val);
}

UCPU_DEFINE_OP(pop) {
	u8 v = ucpu_fetch(cpu);
	u8 t = ucpu_fetch(cpu);
	
	if (!IS(t, uArgType_Memory)) {
		LOG("Argument must be a memory location.");
		return;
	}
	
	if (ustack_empty(cpu->stack)) {
		LOG("Stack is empty.");
		return;
	}
	
	umem_write(cpu->ram, v, ustack_pop(cpu->stack));
}

#define UCPU_DEFINE_BINOP(name, op) \
UCPU_DEFINE_OP(name) { \
	u8 to = ucpu_fetch(cpu); \
	u8 tot = ucpu_fetch(cpu); \
	u8 va = ucpu_fetch(cpu); \
	u8 at = ucpu_fetch(cpu); \
	u8 vb = ucpu_fetch(cpu); \
	u8 bt = ucpu_fetch(cpu); \
	if (!IS(tot, uArgType_Memory)) { \
		LOG("First argument should be a memory location!"); \
		return; \
	} \
	u8 a = IS(at, uArgType_Literal) ? va : umem_read(cpu->ram, va); \
	u8 b = IS(bt, uArgType_Literal) ? vb : umem_read(cpu->ram, vb); \
	u8 v = a op b; \
	umem_write(cpu->ram, to, v); \
}

UCPU_DEFINE_BINOP(add, +)
UCPU_DEFINE_BINOP(sub, -)
UCPU_DEFINE_BINOP(mul, *)
UCPU_DEFINE_BINOP(div, /)
UCPU_DEFINE_BINOP(shl, <<)
UCPU_DEFINE_BINOP(shr, >>)

UCPU_DEFINE_OP(inc) {
	u8 to = ucpu_fetch(cpu);
	u8 tot = ucpu_fetch(cpu);
	if (!IS(tot, uArgType_Memory)) {
		LOG("Argument should be a memory location!");
		return;
	}
	u8 val = umem_read(cpu->ram, to);
	umem_write(cpu->ram, to, val++);
}

UCPU_DEFINE_OP(dec) {
	u8 to = ucpu_fetch(cpu);
	u8 tot = ucpu_fetch(cpu);
	if (!IS(tot, uArgType_Memory)) {
		LOG("Argument should be a memory location!");
		return;
	}
	u8 val = umem_read(cpu->ram, to);
	umem_write(cpu->ram, to, val--);
}

UCPU_DEFINE_OP(call) {
	u8 routine = ucpu_fetch(cpu);
	u8 rt = ucpu_fetch(cpu);
	if (!IS(rt, uArgType_Literal)) {
		LOG("Argument should be a literal!");
		return;
	}
	ustack_push(cpu->call_stack, cpu->pc);
	cpu->pc = routine;
}

UCPU_DEFINE_OP(ret) {
	if (ustack_empty(cpu->call_stack)) {
		LOG("Unexpected return.");
		return;
	}
	cpu->pc = ustack_pop(cpu->call_stack);
}
	

const uOp uCPU_Ops[] = {
	{ "end", op(end) },
	{ "put", op(put) },
	{ "mov", op(mov) },
	{ "jmp", op(jmp) },
	{ "jeq", op(jeq) },
	{ "jne", op(jne) },
	{ "jlt", op(jlt) },
	{ "jgt", op(jgt) },
	{ "jle", op(jle) },
	{ "jge", op(jge) },
	{ "push", op(push) },
	{ "pop", op(pop) },
	{ "add", op(add) },
	{ "sub", op(sub) },
	{ "mul", op(mul) },
	{ "div", op(div) },
	{ "shl", op(shl) },
	{ "shr", op(shr) },
	{ "inc", op(inc) },
	{ "dec", op(dec) },
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

bool uops_op_exists(u16 index) {
	u16 i = 0;
	while (uCPU_Ops[i].name != NULL) {
		if (i == index) return true;
		i++;
	}
	return false;
}