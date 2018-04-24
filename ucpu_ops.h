#ifndef UCPU_OPS_H
#define UCPU_OPS_H

#include "ucpu_core.h"

typedef void (*uOpFunction)(uCPU* cpu);

typedef struct uOp_t {
	const char* name;
	uOpFunction fn;
} uOp;

#define op(name) ucpu_op_##name
#define UCPU_DEFINE_OP(name) static void op(name)(uCPU* cpu)

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

static const uOp uCPU_Ops[] = {
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
	{ NULL, NULL } // guard
};

#endif /* UCPU_OPS_H */

