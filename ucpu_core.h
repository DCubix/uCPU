#ifndef UCPU_CORE_H
#define UCPU_CORE_H

#include "ucpu_types.h"
#include "ucpu_video.h"

#define UCPU_MAX_TICKS 0xFFFF

enum uArgType {
	aLit = (1 << 0),
	aMem = (1 << 1),
	aReg = (1 << 2),
	aAny
};

enum uReg {
	R0 = 0,
	R1, 
	R2,
	R3,
	R4,
	R5,
	R6,
	R7,
	RCount
};

#define IS(v, t) ((v & t) == t)

typedef struct uCPU_t {
	uMem *ram;
	u16 reg[RCount];
	uGfx *gfx;
	u16 pc, ticks, dbptr;
	uStack *call_stack, *stack;
	bool stop;
} uCPU;

uCPU* ucpu_new(u16* program, u16 size);
void ucpu_free(uCPU* cpu);

void ucpu_tick(uCPU* cpu);
u16 ucpu_fetch(uCPU* cpu);

u16* ucpu_fetch_auto(uCPU* cpu, u8 reqtype, u16* out);

void ucpu_run(uCPU* cpu);

#endif // UCPU_CORE_H