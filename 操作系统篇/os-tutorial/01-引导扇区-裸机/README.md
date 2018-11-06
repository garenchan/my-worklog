# 引导扇区-裸机

## 概念

汇编程序、BIOS


## 原理

* 当计算机启动时，BIOS不知道如何加载OS，因此它将该任务委托给引导扇区；

* 因此，引导扇区必须放置在已知的标准位置，这个位置是磁盘的第一个扇区(扇面0，磁头
0，扇区0)，它需要`512`字节；

* 为了确保“磁盘是可引导的”，BIOS会检查所谓的引导扇区中的字节511和512为`0XAA55`；

* 这是有史以来最简单的引导扇区，它基本上都是0，以16位值0XAA55结尾(要注意字节序，
x86是小端字节序)，前3个字节执行无限跳转。
```
e9 fd ff 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
[ 29 more lines with sixteen zero-bytes each ]
00 00 00 00 00 00 00 00 00 00 00 00 00 00 55 aa
```


## 最简单的引导扇区

你可以使用二进制编辑器编写上述的512字节，或者只编写一个非常简单的汇编代码：

```nasm
; Infinite loop (e9 fd ff)
loop:
    jmp loop 

; Fill with 510 zeros minus the size of the previous code
times 510-($-$$) db 0
; Magic number
dw 0xaa55 
```

进行编译：`nasm -f bin boot_sect_simple.asm -o boot_sect_simple.bin`

> OSX warning: if this drops an error, read chapter 00 again

进行验证：`qemu-system-x86_64 boot_sect_simple.bin`

> 如果出现SDL错误，试着传递参数“--nographic”和“--curses”

如果一切顺利的话，你将会看到一个窗口被打开，其中显示“Booting from Hard Disk...”
