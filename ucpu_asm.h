#ifndef UCPU_ASM_H
#define UCPU_ASM_H

#include "ucpu_types.h"

enum pState {
	pState_Error = 0,
	pState_Ready,
	pState_ParseText,
	pState_ParseComment,
	pState_ParseNumber
};

enum tState {
	tState_Error = 0,
	tState_Ready,
	tState_TransformInstr
};

enum pTType {
	pTType_None = 0,
	pTType_Instr,
	pTType_Label,
	pTType_Reg,
	pTType_Mem,
	pTType_Imm
};

typedef struct pLabel_t {
	char name[32];
	u16 addr;
} pLabel;

typedef struct pTok_t {
	char name[32];
	u8 type;
	u16 value;
} pTok;

typedef struct u16Arr_t {
	u16* data;
	u16 len, cap;
} u16Arr;

u16Arr* arr_new();
void arr_add(u16Arr* arr, u16 v);

void uasm_parse_line(char* str);
void uasm_parse(FILE *fp);

u16* uasm_transform(FILE *fp, u16* size);

#endif /* UCPU_ASM_H */

