#include "mem.h"

void memory_copy(u8 *source, u8 *dest, int nbytes) {
    int i;
    for (i = 0; i < nbytes; ++i) {
        *(dest + i) = *(source + i);
    }
}

void memory_set(u8 *dest, u8 val, u32 len) {
    for ( ; len != 0; --len) *dest++ = val;
}

/* 这应该在链接时计算，但是目前使用硬编码也是可以的。
 * 记住我们的内核是从0x1000（在Makefile中定义的）启动的 */
u32 free_mem_addr = 0x1000;
/* 作为一个指向空闲内存的持续增长的指针实现的 */
u32 kmalloc(u32 size, int align, u32 *phys_addr) {
    /* 页面是按照4KB或者0x1000对齐的 */
    if (align == 1 && (free_mem_addr & 0xFFFFF000)) {
        free_mem_addr &= 0xFFFFF000;
        free_mem_addr += 0x1000;
    }

    /* 也保存物理地址 */
    if (phys_addr) *phys_addr = free_mem_addr;

    u32 ret = free_mem_addr;
    free_mem_addr += size; /* 记住要向高地址移动指针 */
    return ret;
}
