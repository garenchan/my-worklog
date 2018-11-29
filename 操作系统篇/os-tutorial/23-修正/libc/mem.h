#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include <stddef.h>

void memory_copy(uint8_t *source, uint8_t *dest, int nbytes);
void memory_set(uint8_t *dest, uint8_t val, uint32_t len);

/* 在这个阶段没有实现“free” */
uint32_t kmalloc(size_t size, int align, uint32_t *phys_addr);

#endif
