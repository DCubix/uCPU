#include "ucpu_asm.h"
#include "ucpu_ops.h"

#include <ctype.h>
#include <string.h>

u16Arr* arr_new() {
	u16Arr* arr = (u16Arr*) malloc(sizeof(u16Arr));
	arr->cap = 10;
	arr->data = (u16*) malloc(sizeof(u16) * arr->cap);
	arr->len = 0;
	return arr;
}

void arr_add(u16Arr* arr, u16 v) {
	if (arr->len >= arr->cap) {
		arr->cap *= 2;
		arr->data = (u16*) realloc(arr->data, sizeof(u16) * arr->cap);
	}
	arr->data[arr->len++] = v;
}

pLabel labels[256];
u16 labelN = 0;

pTok tokens[0x4000];
u16 g_tokens = 0;

int uasm_find_label(const char* name) {
	for (u16 i = 0; i < labelN; i++) {
		if (strcmp(labels[i].name, name) == 0) return labels[i].addr;
	}
	return -1;
}

bool uasm_add_label(const char* name, u16 addr) {
	if (uasm_find_label(name) >= 0) {
		printf("Label redefinition!\n");
		return false;
	}
	pLabel* lbl = &labels[labelN++];
	strncpy(lbl->name, name, 32);
	lbl->addr = addr;
	
	return true;
}

int atoix(char *str) {
	int val;
	if (strncmp(str, "0x", 2) == 0)
		sscanf(str, "%x", &val);
	else
		val = atoi(str);
	return val;
}

void uasm_parse_line(char* str) {
	if (strlen(str) <= 1) return;

//	printf("%s\n", str);
	
	u8 state = pState_Ready;
	u16 token_index = 0;
	bool line_has_label = false;
	bool line_has_instr = false;
	bool do_break = false;
	while (!do_break) {
		switch (state) {
			case pState_Ready: {
				if		(isalpha(*str) || *str == '_' || *str == '.')	state = pState_ParseText;
				else if (*str == ';'  )									state = pState_ParseComment;
				else if (isdigit(*str) || *str == '$' || *str == '[')	state = pState_ParseNumber;
				else if (isspace(*str)) {
					str++;
					break;
				}
				else if (*str == '\0') do_break = true;
				else {
					printf("Invalid character. '%c'\n", *str);
					state = pState_Error;
				}
			} break;
			case pState_Error: do_break = true; break;
			case pState_ParseComment: {
//				printf("[COMMENT]");
				while (*str != '\0') str++;
				state = pState_Ready;
			} break;
			case pState_ParseNumber: {
				u8 type = 0;
				if (*str == '$') {
					type = pTType_Reg;
					str++;
//					printf("[REG]");
				} else if (*str == '[') {
					type = pTType_Mem;
					str++;
//					printf("[MEM]");
				} else {
					type = pTType_Imm;
//					printf("[IMM]");
				}
				char buf[32] = {0};
				u16 i = 0;
				while (!isspace(*str) && *str != '\0' && *str != '[' && str != '$') {
					buf[i] = *str;
					i++;
					str++;
				}
				buf[i] = 0;
				
				pTok* tok = &tokens[g_tokens];
				
				strncpy(tok->name, buf, 32);
				tok->type = type;
				tok->value = atoix(buf);
				
				token_index++;
				g_tokens++;
				state = pState_Ready;
			} break;
			case pState_ParseText: {
				bool new_label = false;
				char buf[32] = {0};
				u16 i = 0;
				while (!isspace(*str) && *str != '\0') {
					if (*str == ':') {
						new_label = true;
						break;
					}
					buf[i] = *str;
					i++;
					str++;
				}
				buf[i] = 0;
				state = pState_Ready;
				
				if (new_label) {
					if (line_has_instr) {
						printf("Label declaration must be the first token.\n");
						state = pState_Error;
						break;
					}
					line_has_label = true;
					uasm_add_label(buf, g_tokens);
//					printf("[DEF_LBL]");
					
					str++;
					break;
				}
				
				pTok* tok = &tokens[g_tokens];
				strncpy(tok->name, buf, 32);
				if (!line_has_instr && (token_index == 0 || (token_index == 1 && line_has_label))) {
					line_has_instr = true;
					tok->type = pTType_Instr;
					tok->value = uops_get_op(buf);
//					printf("[INST]");
				} else {
					tok->type = pTType_Label;
					tok->value = 0;
//					printf("[LBL]");
				}
				token_index++;
				g_tokens++;
			} break;
		}
	}
//	printf("\n");
}

char* readline(FILE *fp, char *buffer) {
	int ch;
	int i = 0;
	size_t buff_len = 0;

	buffer = malloc(buff_len + 1);
	if (!buffer) return NULL;

	while ((ch = fgetc(fp)) != '\n' && ch != EOF) {
		buff_len++;
		void *tmp = realloc(buffer, buff_len + 1);
		if (tmp == NULL) {
			free(buffer);
			return NULL;
		}
		buffer = tmp;

		buffer[i] = (char) ch;
		i++;
	}
	buffer[i] = '\0';

	if (ch == EOF && (i == 0 || ferror(fp))) {
		free(buffer);
		return NULL;
	}
	return buffer;
}

void uasm_parse(FILE *fp) {
	if (fp == NULL) {
		return;
	}

	char *buf;
	while ((buf = readline(fp, NULL)) != NULL) {
		uasm_parse_line(buf);
		free(buf);
	}
}

u16* uasm_transform(FILE *fp, u16* size) {
	g_tokens = 0;
	
	u8 state = tState_Ready;
	
	u16Arr *prog = arr_new();
	uasm_parse(fp);
	
	// Assign label locations
	for (u16 i = 0; i < g_tokens; i++) {
		if (tokens[i].type == pTType_Label) {
			tokens[i].value = uasm_find_label(tokens[i].name);
		}
	}
	
	// Transform instructions (assign form/argtype)
	for (u16 i = 0; i < g_tokens; i++) {
		pTok* tok = &tokens[i];
		switch (state) {
			case tState_Ready: {
				if (tok->type == pTType_Instr) { state = tState_TransformInstr; i--; }
			} break;
			case tState_TransformInstr: {
				if (i+1 >= g_tokens || tokens[i+1].type == pTType_Instr) { // No args
					state = tState_Ready;
					tok->value = ((tok->value & 0xFF) << 8) | (0 & 0xFF);
					break;
				}
				pTok *ptok = tok;
				u16 form = 0;
				tok = &tokens[++i]; u8 ta = tok->type;
				
				if (tokens[i+1].type == pTType_Instr) { // 1 arg
					if (ta == pTType_Imm) {
						form = uArg_I;
					} else if (ta == pTType_Reg) {
						form = uArg_R;
					} else if (ta == pTType_Mem) {
						form = uArg_M;
					}
				} else {
					tok = &tokens[++i]; u8 tb = tok->type;
					if (ta == pTType_Reg && tb == pTType_Imm) {
						form = uArg_RI;
					} else if (ta == pTType_Reg && tb == pTType_Reg) {
						form = uArg_RR;
					} else if (ta == pTType_Reg && tb == pTType_Mem) {
						form = uArg_RM;
					} else if (ta == pTType_Mem && tb == pTType_Imm) {
						form = uArg_MI;
					} else if (ta == pTType_Mem && tb == pTType_Reg) {
						form = uArg_MR;
					} else if (ta == pTType_Mem && tb == pTType_Mem) {
						form = uArg_MM;
					}
				}
				
				u16 pv = ptok->value;
				ptok->value = (((pv & 0xFF) << 8) | (form & 0xFF));
				
				state = tState_Ready;
			} break;
		}
	}
	
	// Convert program
	for (u16 i = 0; i < g_tokens; i++) {
		pTok tok = tokens[i];
		arr_add(prog, tok.value);
	}
	
	u16* program = (u16*) malloc(sizeof(u16) * prog->len);
	memmove(program, prog->data, sizeof(u16) * prog->len);
	
	if (size) *size = prog->len;
	
	free(prog);
	return program;
}