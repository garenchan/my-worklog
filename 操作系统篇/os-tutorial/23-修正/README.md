# 修正


## 概念

- freestanding：这是`gcc`编译器的一个选项，顾名思义就是“不需依靠支撑物的”，在这个
条件下只能使用少量的基本类型和数值范围定义头文件，主要用于裸机开发
- uint32_t：C语言定义的数据类型，无符号32位整型
- size_t：C语言定义的数据类型，无符号整型，位数和平台相关


## 目标

**修正代码中的杂项问题**

OSDev wiki有一个部分[描述了JamesM教程中的一些问题](http://wiki.osdev.org/James_Molloy%27s_Tutorial_Known_Bugs)。于我们遵循了他在第18-22课中
的教程(从中断到malloc)，所以在继续之前，我们需要确保解决了所有问题。


## 正文

### 1. 错误的CFLAGS

我们在编译.o文件时添加`-ffreestanding`，其中包括`kernel_entry.o`、`kernel.bin`和
`os-image.bin`。之前，我们通过使用`-nostdlib`禁用了libgcc（不是lib），并且没有重新
启用它进行链接。由于这很复杂，我们将删除`-nostdlib`。

`nostdinc`也传递给了gcc，但是我们在步骤3中才会用到它，所以让我们删除它。

### 2. kernel.c `main()`函数

修改`kernel/kernel.c`并将`main()`更改为`kernel_main()`，因为gcc将“main”识别为一个
特殊的关键字，我们不想混淆它。

修改`boot/kernel_entry.asm`以指向相应的新名称。

为了修复警告信息`i386-elf-ld: warning: cannot find entry symbol _start; defaulting
to 0000000000001000`，添加一个`global _start`并在`boot/kernel_entry.asm`中定义`_start:`
标签。

### 3. 改造数据类型

定义`u32`等非标准数据类型似乎不是一个好主意，因为C99引入了`uint32_t`等标准固定宽
度数据类型。

我们需要引用`<stdint.h>`，这在`-ffreestanding`（但需要stdlib）下也能工作，并且使
用其中的数据类型而不是我们自己的数据类型，然后在`type.h`中删除它们。

我们还删除了`__asm__`和`__volatile__`周围的下划线，因为它们是没有必要的。

### 4. `kmalloc`的错误对齐

首先，由于`kmalloc`使用了size参数，我们将使用正确的数据类型`size_t`，而不是`u32int_t`。
`size_t`应该用于所有“计数”的参数，并且不能是负的。引用`< stddef.h>`。

我们将在未来修复我们的`kmalloc`，使它成为一个合适的内存管理器并对齐数据类型。现在，
它总是返回一个新的页面对齐内存块。

### 5. 缺失的函数

我们将在下面的课程中实现缺失的`mem*`函数簇。

### 6. 中断处理程序

`cli`是冗余的，因为如果在处理程序中使用`idt_gate_t`的标志启用中断，那么我们已经在
IDT条目上建立了关中断。

`sti`也是冗余的，因为`iret`从栈加载`eflags`值，它包含一个位标识中断是打开还是关闭
的。换句话说，不管中断在此中断之前是否启用，中断处理程序都会自动恢复中断开关。

在`cpu/isr.h`中，`struct registers_t`有几个问题。首先，所谓的`esp`被重命名为`useless`。
之所以它是无用的，因为它和当前栈上下文紧密相关，而不是被中断的内容。然后，我们将
`useresp`重命名为`esp`。

我们在`cpu/interrupt.asm`中`call isr_handler`之前添加`cld`，这也是OSDev wiki建议的。

关于`cpu/interrupt.asm`还有最后一个重要的问题。公共桩在栈上创建一个`struct registers_t`
实例，然后调用C处理程序。但是这会破坏ABI，因为栈属于被调用的函数，它们可以根据需
要对栈进行修改。需要将结构体作为指针进行传递。

为此，请编辑`cpu/isr.h`和`cpu/isr.c`，并将`registers_t r`改为`registers_t *t`，然
后就不是通过`.`访问结构的字段了，而是通过`->`访问指针的字段。最后，在`cpu/interrupt.asm`
中调用`isr_handler`和`irq_handler`之前添加一个`push esp`--记得在调用完成之后`pop eax`
来清除指针。

当前的回调（计时器和键盘）也需要修改，以便使用指向`registers_t`的指针。
