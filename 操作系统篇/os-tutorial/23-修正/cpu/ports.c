#include "ports.h"

/**
 * 从指定端口读取一个字节
 */
uint8_t port_byte_in (uint16_t port) {
    uint8_t result;
    /* 内联汇编定义
     * !! 注意与NASM不同的是，源寄存器和目的寄存器的顺序被交换了
     *
     * '"=a" (result)'：用于将C变量'(result)'设置为寄存器e'a'x的值
     * '"d" (port)'：映射C变量'(port)'到寄存器e'd'x
     *
     * 输入和输出用冒号分隔
     */
     asm("in %%dx, %%al" : "=a" (result) : "d" (port));
     return result;
}

void port_byte_out (uint16_t port, uint8_t data) {
    /* 注意这里的2个寄存器都被映射到C变量，并且没有返回任何东西，因此
     * 在汇编定义中没有出现'='。
     * 但是我们看到了一个逗号，因为输入区有两个变量，在“返回”区域内
     * 什么也没有
     */
    asm("out %%al, %%dx" : : "a" (data), "d" (port));
}

uint16_t port_word_in (uint16_t port) {
    uint16_t result;
    asm("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

void port_word_out (uint16_t port, uint16_t data) {
    asm("out %%ax, %%dx" : : "a" (data), "d" (port));
}
