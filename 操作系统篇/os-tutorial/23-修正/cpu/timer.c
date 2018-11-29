#include "timer.h"
#include "isr.h"
#include "ports.h"
#include "../libc/function.h"


uint32_t tick = 0;

static void timer_callback(registers_t *regs) {
    tick++;
    UNUSED(regs);
}

void init_timer(uint32_t freq) {
    /* 安装我们刚才写的函数 */
    register_interrupt_handler(IRQ0, timer_callback);

    /* 获取PIT(可编程间隔定时器)值：1193180 HZ的硬件时钟 */
    uint32_t divisor = 1193180 / freq;
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
    /* 发送指令 */
    port_byte_out(0x43, 0x36); /* 指令端口 */
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}
