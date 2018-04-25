#include "ucpu_ops.h"

UCPU_DEFINE_OP(end) {
	cpu->stop = true;
}

UCPU_DEFINE_OP(put) {
	u16 ch = 0; ucpu_fetch_auto(cpu, aAny, &ch);
	putc(ch, stdout);
}

UCPU_DEFINE_OP(mov) {
	u16* dest = ucpu_fetch_auto(cpu, aMem | aReg, NULL);
	u16 src = 0; ucpu_fetch_auto(cpu, aAny, &src);
	if (dest) *dest = src;
}

UCPU_DEFINE_OP(jmp) {
	u16 to = 0;
	ucpu_fetch_auto(cpu, aLit, &to);
	
	cpu->pc = to;
}

#define UCPU_DEFINE_COND_JMP(name, op) \
UCPU_DEFINE_OP(name) { \
	u16 a, b, to; \
	ucpu_fetch_auto(cpu, aAny, &a); \
	ucpu_fetch_auto(cpu, aAny, &b); \
	ucpu_fetch_auto(cpu, aLit, &to); \
	if (a op b) cpu->pc = to; \
}

UCPU_DEFINE_COND_JMP(jeq, ==)
UCPU_DEFINE_COND_JMP(jne, !=)
UCPU_DEFINE_COND_JMP(jlt, <)
UCPU_DEFINE_COND_JMP(jgt, >)
UCPU_DEFINE_COND_JMP(jle, <=)
UCPU_DEFINE_COND_JMP(jge, >=)

UCPU_DEFINE_OP(push) {
	u16 val; ucpu_fetch_auto(cpu, aAny, &val);
	ustack_push(cpu->stack, val);
}

UCPU_DEFINE_OP(pop) {
	u16* v = ucpu_fetch_auto(cpu, aReg | aMem, NULL);
	
	if (ustack_empty(cpu->stack)) {
		LOG("Stack is empty.");
		return;
	}
	
	if (v) *v = ustack_pop(cpu->stack);
}

#define UCPU_DEFINE_BINOP(name, op) \
UCPU_DEFINE_OP(name) { \
	u16 a, b; \
	u16* to = ucpu_fetch_auto(cpu, aReg | aMem, NULL); \
	ucpu_fetch_auto(cpu, aAny, &a); \
	ucpu_fetch_auto(cpu, aAny, &b); \
	if (to) *to = a op b; \
}

UCPU_DEFINE_BINOP(add, +)
UCPU_DEFINE_BINOP(sub, -)
UCPU_DEFINE_BINOP(mul, *)
UCPU_DEFINE_BINOP(div, /)
UCPU_DEFINE_BINOP(shl, <<)
UCPU_DEFINE_BINOP(shr, >>)

UCPU_DEFINE_OP(inc) {
	u16* to = ucpu_fetch_auto(cpu, aReg | aMem, NULL); \
	if (to) (*to)++;
}

UCPU_DEFINE_OP(dec) {
	u16* to = ucpu_fetch_auto(cpu, aReg | aMem, NULL); \
	if (to) (*to)--;
}

UCPU_DEFINE_OP(call) {
	u16 routine; ucpu_fetch_auto(cpu, aLit, &routine);
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

UCPU_DEFINE_OP(flip) {
	ugfx_flip(cpu->gfx);
}

UCPU_DEFINE_OP(cls) {
	u16 color = 0; ucpu_fetch_auto(cpu, aAny, &color);
	ugfx_clear(cpu->gfx, color);
}

UCPU_DEFINE_OP(vmov) { // move value to video/screen memory
	u16 vto; ucpu_fetch_auto(cpu, aAny, &vto);
	u16 vval; ucpu_fetch_auto(cpu, aAny, &vval);
	if (vval >= 4) return;
	umem_write(cpu->gfx->vram, vto, vval);
}

UCPU_DEFINE_OP(vdrw) { // video draw 4x4 sprite
	u16 vx; ucpu_fetch_auto(cpu, aAny, &vx);
	u16 vy; ucpu_fetch_auto(cpu, aAny, &vy);
	u16 spr; ucpu_fetch_auto(cpu, aLit | aReg, &spr);
	
	u16 i = spr;
	for (u16 y = 0; y < 4; y++) {
		for (u16 x = 0; x < 4; x++) {
			u16 col = umem_read(cpu->ram, i++);
			if (col >= 4) continue;
			ugfx_set(cpu->gfx, vx+x, vy+y, col);
		}
	}
}

UCPU_DEFINE_OP(db) { // define byte/short, simplified form! db 0x3f5f
	u16 v = ucpu_fetch(cpu);
	umem_write(cpu->ram, cpu->dbptr++, v);
}

UCPU_DEFINE_OP(dq) { // define four bytes/shorts, simplified form! db 0x3 0x3 0x4 0x4
	u16 a = ucpu_fetch(cpu);
	u16 b = ucpu_fetch(cpu);
	u16 c = ucpu_fetch(cpu);
	u16 d = ucpu_fetch(cpu);
	umem_write(cpu->ram, cpu->dbptr++, a);
	umem_write(cpu->ram, cpu->dbptr++, b);
	umem_write(cpu->ram, cpu->dbptr++, c);
	umem_write(cpu->ram, cpu->dbptr++, d);
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
	{ "vmov", op(vmov) },
	{ "db", op(db) },
	{ "dq", op(dq) },
	{ "vdrw", op(vdrw) },
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