# 引导扇区-打印

## 概念

中断、CPU寄存器


## 目标

**让我们以前安静的引导扇区打印一些文本**


## 打印文本的引导扇区

我们将改进我们的无限循环引导扇区，并在屏幕上打印一些东西。我们将为此引发一个中断。

在这个示例中，我们将把“Hello”单词的每个字符写入寄存器`al`(`ax`的下部)，将字节`0x0e`写入
`ah`(`ax`的上部)，并引发中断`0x10`，这是视频服务的通用中断。

`ah`上的`0x0e`告诉视频中断，我们要运行的实际功能是“在tty模式下写入`al`的内容”。

我们只会设置tty模式一次，但在现实世界中我们不能确定`ah`的内容是常量。当我们进入睡眠
状态时一些其他的进程可能会在CPU上运行，并且没有进行合适的清理工作以至于垃圾数据残留
在`ah`上。

对于本例，我们不需要处理这个问题，因为我们是CPU上唯一运行的东西。

我们的新引导扇区如下：

```nasm
mov ah, 0x0e ; tty mode
mov al, 'H'
int 0x10
mov al, 'e'
int 0x10
mov al, 'l'
int 0x10
int 0x10 ; 'l' is still on al, remember?
mov al, 'o'
int 0x10

jmp $ ; jump to current address = infinite loop

; padding and magic number
times 510 - ($-$$) db 0
dw 0xaa55 
```

你可以使用`xxd file.bin`检查二进制数据。

无论如何，你知道这个练习：

`nasm -fbin boot_sect_hello.asm -o boot_sect_hello.bin`

`qemu-system-x86_64 boot_sect_hello.bin`

你的引导扇区会打印“Hello”然后进入一个无限循环。
