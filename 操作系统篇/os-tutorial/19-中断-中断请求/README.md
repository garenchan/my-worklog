# 中断-中断请求


## 概念

- IRQs：Interrupt Request，中断请求
- PIC：Programmable Interrupt Controller，可编程中断控制器
- polling：轮询


## 目标

**完成中断实现和CPU定时器**

当CPU引导时，PIC将IRQs 0-7映射到INT 0x8-0xF，将IRQs 8-15映射到INT 0x70-0x77。这与
我们上节课编写的ISRs有所冲突。由于我们对ISRs 0-31进行了编程，所以将IRQs重
新映射到ISRs 32-47是标准的。

PIC之间通过I/O端口进行通信（参见第15课）。主PIC有命令0x20和数据0x21，而从PIC有命
令0xA0和数据0xA1。

重新映射PIC的代码很奇怪，还包含一些掩码，如果你感兴趣，请[查看本文](https://wiki.osdev.org/PIC)。否则，
只用看`cpu/isr.c`中我们为ISRs设置IDT门后面的新代码。然后，我们为IRQs添加IDT门。

现在我们跳到汇编代码`interrupt.asm`。第一个任务是为我们刚刚在C代码中使用的IRQ符号
添加全局定义。看看`global`语句的结尾。

然后，添加IRQ处理程序。同样也是在`interrupt.asm`的底部。注意它们是如何跳转到一个新
的公共树桩的：`irq_common_stub`（下一步）。

然后我们创建与ISR中那个非常相似的`irq_common_stub`。它位于`interrupt.asm`的顶部，
并且它也定义了一个新的`[extern irq_handler]`。

现在回到C代码来编写`isr.c`中的`irq_handler()`。它向PICs发送一些EOIs并调用适当的处
理程序，这些处理程序存储在名为`interrupt_handlers`的数组中，并在文件顶部定义。新的
结构在`isr.h`中定义。我们还将使用一个简单的函数来注册中断处理程序。

这些包含了大量的工作，但现在我们可以定义我们的第一个IRQ处理程序！

`kernel.c`中没有任何变化，因此没有什么新东西可以运行和查看。请继续下一课，检查那
些闪亮的新IRQs。
