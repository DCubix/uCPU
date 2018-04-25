#ifndef UCPU_OPS_H
#define UCPU_OPS_H

#include "ucpu_core.h"

#include <strings.h>

typedef void (*uOpFunction)(uCPU* cpu);

typedef struct uOp_t {
	const char* name;
	uOpFunction fn;
} uOp;

#define op(name) ucpu_op_##name
#define UCPU_DEFINE_OP(name) static void op(name)(uCPU* cpu)
	
extern const uOp uCPU_Ops[];

u16 uops_get_op(const char* name);
bool uops_op_exists(u16 index);

#define UOP(name) uops_get_op(name)

#endif /* UCPU_OPS_H */

