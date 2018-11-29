#include "mem.h"

void memory_copy(uint8_t *source, uint8_t *dest, int nbytes) {
    int i;
    for (i = 0; i < nbytes; ++i) {
        *(dest + i) = *(source + i);
    }
}

void memory_set(uint8_t *dest, uint8_t val, uint32_t len) {
    for ( ; len != 0; --len) *dest++ = val;
}

/* 这应该在链接时计算，但是目前使用硬编码也是可以的。
 * 记住我们的内核是从0x1000（在Makefile中定义的）启动的 */
uint32_t free_mem_addr = 0x1000;
/* 作为一个指向空闲内存的持续增长的指针实现的 */
uint32_t kmalloc(size_t size, int align, uint32_t *phys_addr) {
    /* 页面是按照4KB或者0x1000对齐的 */
    if (align == 1 && (free_mem_addr & 0xFFFFF000)) {
        free_mem_addr &= 0xFFFFF000;
        free_mem_addr += 0x1000;
    }

    /* 也保存物理地址 */
    if (phys_addr) *phys_addr = free_mem_addr;

    uint32_t ret = free_mem_addr;
    free_mem_addr += size; /* 记住要向高地址移动指针 */
    return ret;
}
