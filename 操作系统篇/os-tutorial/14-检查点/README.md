# 检查点


## 概念

宏内核、微内核、调试器、GDB


## 目标

**暂停并稍微组织一下代码，然后学习如何用gdb调试内核**


## CheckPoint

也许你没有意识到，但是你已经运行了自己的内核!

然而，它做的很少，只是打印一个“X”。现在是时候停下来，将代码组织到文件夹中，为将来
的代码创建可伸缩的Makefile，并考虑策略了。

看看新的文件夹结构。大多数文件都已从以前的课程中进行了符号链接，因此如果我们必须
在某个时候更改它们，最好删除符号链接并创建一个新文件。

此外，从现在开始，我们将主要使用C来编写代码，我们将利用qemu的能力来打开到gdb的连
接。首先，让我们安装一个交叉编译的gdb，因为OSX使用的`lldb`与ELF文件格式不兼容(在
Homebrew的repos上也没有gdb)。

```shell
cd /tmp/src
curl -O http://ftp.rediris.es/mirror/GNU/gdb/gdb-7.8.tar.gz
tar xf gdb-7.8.tar.gz
mkdir gdb-build
cd gdb-build
export PREFIX="/usr/local/i386elfgcc"
export TARGET=i386-elf
../gdb-7.8/configure --target="$TARGET" --prefix="$PREFIX" --program-prefix=i386-elf-
make
make install
```

检查Makefile目标`make debug`。这个目标用于构建`kernel.elf`，它是一个带有我们在内
核中生成的所有符号的对象文件(不是二进制文件)，这要归功于gcc的`-g`标志。请用`xxd`
检查它，你将会看到一些字符串。实际上，检查对象文件中的字符串的正确方法是使用`strings kernel.elf`。

我们可以利用qemu这个很酷的特性。键入`make debug`，然后在gdb shell上：

- 在`kernel.c:main()`上设置断点：`b main`
- 运行OS：`continue`
- 在代码中运行两步：`next`和`next`。你将看到我们即将在屏幕上打印的'X'，但它还没有
出现(查看qemu屏幕)
- 让我们看看显存中有什么：`print *video_memory`。有一个来自于“Landed in 32-bit Protected Mode”的‘L’
- 嗯，让我们确保`video_memory`指向正确的地址：`print video_memory`
- 执行`next`来显示我们的‘X’
- 让我们再确认一次：`print *video_memory`并查看qemu屏幕。它肯定在那里。

现在是阅读一些`gdb`教程的好机会，并从中学习到超级有用的东西，例如`info registers`，
这将在未来节省我们很多时间！

你可能注意到，因为这是一个教程，所以我们还没有讨论要编写哪种内核。它将可能是一个
宏内核，因为它更容易设计和实现，毕竟这也是我们的第一个操作系统。也许将来我们会添
加一节微内核设计课程“15-b”。谁知道呢。
