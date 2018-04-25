#include "ucpu_ops.h"

UCPU_DEFINE_OP(end) {
	cpu->stop = true;
}

UCPU_DEFINE_OP(put) {
	u8 ch = 0;
	ucpu_fetch_auto(cpu, uArgType_Any, &ch);
	putc(ch, stdout);
}

UCPU_DEFINE_OP(mov) {
	u8* dest = ucpu_fetch_auto(cpu, uArgType_Memory | uArgType_Register, NULL);
	u8 src = 0; ucpu_fetch_auto(cpu, uArgType_Any, &src);
	if (dest) *dest = src;
}

UCPU_DEFINE_OP(jmp) {
	u8 to = 0;
	ucpu_fetch_auto(cpu, uArgType_Literal, &to);
	
	cpu->pc = to;
}

#define UCPU_DEFINE_COND_JMP(name, op) \
UCPU_DEFINE_OP(name) { \
	u8 a, b, to; \
	ucpu_fetch_auto(cpu, uArgType_Any, &a); \
	ucpu_fetch_auto(cpu, uArgType_Any, &b); \
	ucpu_fetch_auto(cpu, uArgType_Literal, &to); \
	if (a op b) cpu->pc = to; \
}

UCPU_DEFINE_COND_JMP(jeq, ==)
UCPU_DEFINE_COND_JMP(jne, !=)
UCPU_DEFINE_COND_JMP(jlt, <)
UCPU_DEFINE_COND_JMP(jgt, >)
UCPU_DEFINE_COND_JMP(jle, <=)
UCPU_DEFINE_COND_JMP(jge, >=)

UCPU_DEFINE_OP(push) {
	u8 val; ucpu_fetch_auto(cpu, uArgType_Any, &val);
	ustack_push(cpu->stack, val);
}

UCPU_DEFINE_OP(pop) {
	u8* v = ucpu_fetch_auto(cpu, uArgType_Register | uArgType_Memory, NULL);
	
	if (ustack_empty(cpu->stack)) {
		LOG("Stack is empty.");
		return;
	}
	
	if (v) *v = ustack_pop(cpu->stack);
}

#define UCPU_DEFINE_BINOP(name, op) \
UCPU_DEFINE_OP(name) { \
	u8 a, b; \
	u8* to = ucpu_fetch_auto(cpu, uArgType_Register | uArgType_Memory, NULL); \
	ucpu_fetch_auto(cpu, uArgType_Any, &a); \
	ucpu_fetch_auto(cpu, uArgType_Any, &b); \
	if (to) *to = a op b; \
}

UCPU_DEFINE_BINOP(add, +)
UCPU_DEFINE_BINOP(sub, -)
UCPU_DEFINE_BINOP(mul, *)
UCPU_DEFINE_BINOP(div, /)
UCPU_DEFINE_BINOP(shl, <<)
UCPU_DEFINE_BINOP(shr, >>)

UCPU_DEFINE_OP(inc) {
	u8* to = ucpu_fetch_auto(cpu, uArgType_Register | uArgType_Memory, NULL); \
	if (to) (*to)++;
}

UCPU_DEFINE_OP(dec) {
	u8* to = ucpu_fetch_auto(cpu, uArgType_Register | uArgType_Memory, NULL); \
	if (to) (*to)--;
}

UCPU_DEFINE_OP(call) {
	u8 routine; ucpu_fetch_auto(cpu, uArgType_Literal, &routine);
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

UCPU_DEFINE_OP(dat) {
	// dat [mem] 0 0 0 0 0 0 0 0
	for (u8 i = 0; i < 8; i++) {
		u8 v = ucpu_fetch(cpu);
		
	}
}

UCPU_DEFINE_OP(flip) {
	ugfx_flip(cpu->gfx);
}

UCPU_DEFINE_OP(cls) {
	u8 color = 0; ucpu_fetch_auto(cpu, uArgType_Any, &color);
	ugfx_clear(cpu->gfx, color);
}

UCPU_DEFINE_OP(vpok) {
	u8 x; ucpu_fetch_auto(cpu, uArgType_Any, &x);
	u8 y; ucpu_fetch_auto(cpu, uArgType_Any, &y);
	u8 vcol; ucpu_fetch_auto(cpu, uArgType_Any, &vcol);
	umem_write(cpu->gfx->vram, x + y * UCPU_VIDEO_WIDTH, vcol);
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
	{ "flip", op(flip) },
	{ "cls", op(cls) },
	{ "vpok", op(vpok) },
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