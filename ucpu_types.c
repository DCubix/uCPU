#include <string.h>

#include "ucpu_types.h"

uStack* ustack_new() {
	uStack* stack = (uStack*) malloc(sizeof(uStack));
	stack->top = -1;
	memset(stack->elements, 0, sizeof(u16) * UCPU_USTACK_MAX_SIZE);
	return stack;
}

void ustack_push(uStack* stack, u16 element) {
	if (ustack_full(stack)) {
		LOG("Stack overflow!");
		return;
	}
	stack->elements[++stack->top] = element;
}

u16 ustack_pop(uStack* stack) {
	if (ustack_empty(stack)) {
		LOG("Stack is empty!");
		return 0;
	}
	return stack->elements[stack->top--];
}

bool ustack_empty(uStack* stack) {
	return stack->top < 0;
}

bool ustack_full(uStack* stack) {
	return stack->top >= UCPU_USTACK_MAX_SIZE-1;
}

uMem* umem_new(u16 size) {
	uMem* mem = (uMem*) malloc(sizeof(uMem));
	mem->data = (u16*) malloc(size * sizeof(u16));
	mem->size = size;
	memset(mem->data, 0, size * sizeof(u16));
	return mem;
}

void umem_free(uMem* mem) {
	free(mem->data);
	mem->size = 0;
}

u16 umem_read(uMem* mem, u16 addr) {
	return mem->data[addr];
}

void umem_write(uMem* mem, u16 addr, u16 v) {
	mem->data[addr] = v;
}

void umem_dump(uMem* mem, FILE* fp) {
	if (!fp || (fp && ferror(fp))) {
		LOG("Invalid file.");
		return;
	}
	
	fwrite(mem->data, sizeof(u16), mem->size, fp);
}
