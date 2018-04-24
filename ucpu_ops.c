#include "ucpu_ops.h"

u16 uops_get_op(const char* name) {
	u16 i = 0;
	while (uCPU_Ops[i].name != NULL) {
		if (strcasecmp(name, uCPU_Ops[i].name) == 0) return i;
		i++;
	}
	return 0;
}