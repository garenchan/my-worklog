#include "timer.h"
#include "isr.h"
#include "ports.h"
#include "../libc/function.h"


u32 tick = 0;

static void timer_callback(registers_t regs) {
    tick++;
    UNUSED(regs);
}

void init_timer(u32 freq) {
    /* 安装我们刚才写的函数 */
    register_interrupt_handler(IRQ0, timer_callback);

    /* 获取PIT(可编程间隔定时器)值：1193180 HZ的硬件时钟 */
    u32 divisor = 1193180 / freq;
    u8 low = (u8)(divisor & 0xFF);
    u8 high = (u8)((divisor >> 8) & 0xFF);
    /* 发送指令 */
    port_byte_out(0x43, 0x36); /* 指令端口 */
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}
