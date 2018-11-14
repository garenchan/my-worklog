# 内核-裸机


## 概念

内核、ELF格式、Makefile


## 目标

**创建一个简单的内核和一个能够引导它的引导扇区**


## 内核

我们的C内核只会在屏幕的左上角打印一个“X”。继续并打开`kernel.c`。

您会注意到一个不做任何事情的dummy函数。这个函数将迫使我们创建一个内核入口例程，它
不指向内核中的字节0x0，而是指向一个我们知道会启动它的实际标签。在本例中，也就是函
数`main()`。

`i386-elf-gcc -ffreestanding -c kernel.c -o kernel.o`

该例程是在`kernel_entry.asm`上编码的，阅读它，你将了解到如何在汇编语言中使用`[extern]`
声明。为了编译这个文件，我们将生成一个`elf`格式文件，而不是生成一个二进制文件，该
文件将与`kernel.o`进行链接。

`nasm kernel_entry.asm -f elf -o kernel_entry.o`


## 链接器

链接器是一个非常强大的工具，我们将开始从中受益。

要将两个对象文件链接到单个二进制内核并解析标签引用，请运行：

`i386-elf-ld -o kernel.bin -Ttext 0x1000 kernel_entry.o kernel.o --oformat binary`

注意，我们的内核不会放在内存地址`0x0`，而是放在`0x1000`。引导扇区也需要知道这个地址。


## 引导扇区

这和第10课的内容很相似。打开`bootsect.asm`并检查代码。实际上，如果不去掉用于在屏幕上
打印消息的代码行，就会有几十行。

使用命令`nasm bootsect.asm -f bin -o bootsect.bin`编译它。


## 合并

现在怎么办呢？我们有两个单独的文件：引导扇区和内核？

难道我们就不能把它们“链接”到单个文件中吗？是的，我们可以，而且很简单：

`cat bootsect.bin kernel.bin > os-image.bin`


## 运行

现在我们可以使用qemu运行`os-image.bin`了。

记住，如果发现磁盘加载错误，那么你可能需要使用磁盘号或qemu参数(软盘= 0x0，hdd = 0x80)。
我通常使用`qemu-system-i386 -fda os-image.bin`。

你将看到4条信息：

- Started in 16-bit Real Mode
- Loading kernel into memory
- (左上角) "Landed in 32-bit Protected Mode"
- (左上角，覆盖先前的信息) "X"

恭喜你！


## Makefile

作为最后一步，我们将使用Makefile整理编译过程。打开`Makefile`脚本并检查其内容。如
果你不知道Makefile是什么，现在是进行谷歌和学习它的好机会，因为这将为我们在未来节
省很多时间。

> 注意Makefile里面的缩进需要使用`tab`而不能是空格
