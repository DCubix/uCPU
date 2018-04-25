#include <string.h>

#include "ucpu_types.h"

uStack* ustack_new() {
	uStack* stack = (uStack*) malloc(sizeof(uStack));
	stack->top = -1;
	memset(stack->elements, 0, sizeof(u8) * UCPU_USTACK_MAX_SIZE);
	return stack;
}

void ustack_push(uStack* stack, u8 element) {
	if (ustack_full(stack)) {
		LOG("Stack overflow!");
		return;
	}
	stack->elements[++stack->top] = element;
}

u8 ustack_pop(uStack* stack) {
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
	mem->data = (u8*) malloc(size);
	mem->size = size;
	memset(mem->data, 0, size);
	return mem;
}

void umem_free(uMem* mem) {
	free(mem->data);
	free(mem);
	mem->size = 0;
}

u8 umem_read(uMem* mem, u16 addr) {
	return mem->data[addr];
}

void umem_write(uMem* mem, u16 addr, u8 v) {
	mem->data[addr] = v;
}

u16 umem_read16(uMem* mem, u16 addr) {
	u16 value = 0;
	memcpy(&value, mem->data + addr, sizeof(u16));
	return value;
}

void umem_write16(uMem* mem, u16 addr, u16 v) {
	memset(mem->data + addr, v, sizeof(u16));
}

void umem_dump(uMem* mem, FILE* fp) {
	if (!fp || (fp && ferror(fp))) {
		LOG("Invalid file.");
		return;
	}
	
	fwrite(mem->data, sizeof(u8), mem->size, fp);
}