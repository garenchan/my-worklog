#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/* 段选择器 */
#define KERNEL_CS 0x08

/* 每个中断门(处理程序)是如何定义的 */
typedef struct {
    uint16_t low_offset; /* 处理程序地址的低16位 */
    uint16_t sel;        /* 内核段选择器 */
    uint8_t always0;
    /* 第一个字节
     * 第7位：中断出现
     * 6-5位：调用者的特权级别（0=内核..3=用户）
     * 第4位：为中断门设置为0
     * 3-0位：二进制1110 = 十进制的，14 = 32位中断门 */
    uint8_t flags;
    uint16_t high_offset; /* 处理程序地址的高16位 */
} __attribute__((packed)) idt_gate_t;

/* 一个指向中断处理程序数组的指针，汇编指令“lidt”用于读取它。*/
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_register_t;

#define IDT_ENTRIES 256
idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;


/* 在idt.c中实现的函数 */
void set_idt_gate(int n, uint32_t handler);
void set_idt();

#endif
