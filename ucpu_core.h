#ifndef UCPU_CORE_H
#define UCPU_CORE_H

#include "ucpu_types.h"

#define UCPU_MAX_TICKS 0xFFFF

enum uArgType {
	uArgType_Literal = (1 << 0),
	uArgType_Memory = (1 << 1)
};

#define IS(v, t) ((v & t) == t)

typedef struct uCPU_t {
	uMem *ram;
	u16 pc, ticks;
	uStack *call_stack, *stack;
	bool stop;
} uCPU;

extern uCPU* ucpu_new(u8* program, u16 size);
extern void ucpu_free(uCPU* cpu);

extern void ucpu_tick(uCPU* cpu);
extern u8 ucpu_fetch(uCPU* cpu);
extern u16 ucpu_fetch16(uCPU* cpu);

extern void ucpu_run(uCPU* cpu);

#endif // UCPU_CORE_H