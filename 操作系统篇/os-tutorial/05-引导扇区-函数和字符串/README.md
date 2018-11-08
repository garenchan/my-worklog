# 引导扇区-函数和字符串


## 概念

控制结构、函数调用、字符串


## 目标

**学习如何用汇编程序编写基本内容(循环、函数)**

我们越来越接近我们的最终引导扇区。

在第7课中，我们将开始读磁盘，这是加载内核之前的最后一步。但首先，我们将编写一些
带有控制结构、函数调用和完整字符串使用的代码。在讨论磁盘和内核之前，我们确实需要
熟悉这些概念。


## 字符串

像字节一样定义字符串，但是用空字节(是的，像C)终止它们，以便能够确定它们的结束。

```nasm
mystring:
    db 'Hello, World', 0
```

注意，用引号括起来的文本由汇编程序转换为ASCII，而单独的0将作为字节`0x00`(空字节)
传递。


## 控制结构

我们已经使用了一个：`jmp $`用于无限循环。

汇编程序跳转由前面的指令结果定义。例如：

```nasm
cmp ax, 4     ; if ax = 4
je ax_is_four ; do something (by jumping to that label)
jmp else      ; else, do another thing
jmp endif     ; finally, resume the normal flow

ax_is_four:
    .....
    jmp endif

else:
    .....
    jmp endif ; not actually necessary but printed here for completeness

endif:
```
在你的头脑中高层次地思考，然后用这种方式把它转换成汇编程序。

有许多`jmp`条件:如果相等，如果小于，等等。它们非常直观，但是您总是可以谷歌它们。


## 函数调用

正如你可能认为的，调用函数只是跳转到标签。

棘手的部分是参数。使用参数有两个步骤:

1. 程序员知道他们共享一个特定的寄存器或内存地址；
2. 编写更多的代码，使函数调用更通用和没有副作用

第一步很简单。我们将约定使用`al`(实际上是`ax`)作为参数。

```nasm
mov al, 'X'
jmp printed
endprint:

...

print:
    mov ah, 0x0e ; tty code
    int 0x10     ; I assume that 'al' already has the character
    jmp endprint ; this label is also pre-agreed
```

你会看到，这种方法很快就会变成意大利面条式的代码。当前的`print`函数只会返回到`endprint`。
如果其他函数想调用它呢？我们正在扼杀代码的重用性。

正确的解决方案提供了两个改进：

- 我们将存储返回地址，以便它可能有所不同；
- 我们将保存当前寄存器以允许子函数修改它们而不产生任何副作用

为了存储返回地址，CPU将会帮助我们。不要使用两个`jmp`来调用子例程，而是使用`call`
和`ret`。

为了保存寄存器数据，还有一个使用堆栈的特殊命令：`pusha`及其兄弟`popa`，它将所有
寄存器自动推送到栈中，然后恢复它们。


## 引入外部文件

首先假设你是一个程序员，那么不需要说服你为什么这是一个好主意。

语法如下：

```nasm
%include "file.asm"
```


## 打印十六进制值

下节课我们将开始从磁盘读取数据，所以我们需要一些方法来确保我们正在读取正确的数据。
文件`boot_sect_print_hex.asm`扩展自`boot_sect_print.asm`以打印十六进制字节，而不
仅仅是ASCII字符。


## 代码

让我们跳到代码部分。文件`boot_sect_print.asm`是主文件中通过`%include`引入的子程
序。它使用循环在屏幕上打印字节。它还包含一个打印换行符的函数。我们熟悉的 `'\n'` 
实际上是两个字节，换行字符`0x0A`和回车字符`0x0D`。请去掉回车字符进行实验，看看效
果如何。

如上所述，`boot_sect_print_hex.asm`允许打印字节。

主文件`boot_sect_main.asm`加载一对字符串和字节，调用`print`和`print_hex`并挂起。
如果你理解了前面的部分，就会明白的。
