#include "../cpu/isr.h"
#include "../cpu/timer.h"
#include "../drivers/keyboard.h"

void main() {
    isr_install();

    asm volatile("sti");
    init_timer(50);
    /* 可以注释掉定时器的IRQ处理程序，这样读取观察键盘IRQs会更容易一些 */
    init_keyboard();
}
