#ifndef UCPU_OPS_H
#define UCPU_OPS_H

#include "ucpu_core.h"

#include <strings.h>

typedef void (*uOpFunction)(uCPU*, u8);

typedef struct uOp_t {
	const char* name;
	uOpFunction fn;
} uOp;

#define op(name) ucpu_op_##name
#define UCPU_DEFINE_OP(name) static void op(name)(uCPU* cpu, u8 form)
	
extern const uOp uCPU_Ops[];

u16 uops_get_op(const char* name);
const char* uops_get_op_name(u16 id);
bool uops_op_exists(u16 index);

#define UOP(name, form) (((uops_get_op(name) & 0xFF) << 8) | (form & 0xFF))
#define UOPN(name) UOP(name, 0)

enum uSysCalls {
	uSys_Reset = 0x0000,
	uSys_Gfx_Clear,
	uSys_Gfx_Flip,
};

enum uArgForm {
	uArg_None = 0,
	
	// Binary
	uArg_RI = 0xA1, // mov r0 100
	uArg_RR,		// mov r0 r1
	uArg_RM,		// mov r0 [0x3456]
	uArg_MI,		// mov [0x3456] 100
	uArg_MR,		// mov [0x3456] r1
	uArg_MM,		// mov [0x3456] [0x24]
	
	// Unary
	uArg_R = 0xB1,	// inc r0
	uArg_M,			// inc [0x34fd]
	uArg_I,			// db 42
};

#endif /* UCPU_OPS_H */

