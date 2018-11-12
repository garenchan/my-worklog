# 内核-交叉编译


## 概念

交叉编译：在一个平台上生成另一个平台上的可执行代码


## 目标

**创建一个开发环境来构建你的内核**

无论如何，一旦我们开始用更高级的语言进行开发，即c语言，我们将需要一个交叉编译器。
我们可以从这里了解到[为什么需要一个交叉编译器](https://wiki.osdev.org/Why_do_I_need_a_Cross_Compiler)。


## 依赖包

首先，安装依赖包。在Linux上，请使用自带的包管理器进行安装；在Mac上，请先确保安装了
[brew](http://brew.sh/)，接着就可以使用`brew install`来安装这些软件包。

- gmp
- mpfr
- libmpc
- gcc

是的，我们将需要`gcc`来构建我们的交叉编译的`gcc`，特别是在gcc被弃用为`clang`的Mac上。

安装后，找到打包好的gcc在哪里(记住，不是clang)并设置为环境变量。例如:

```shell
export CC=/usr/local/bin/gcc-4.9
export LD=/usr/local/bin/gcc-4.9
```

我们将需要构建binutils和交叉编译的gcc，并将它们放入`/usr/local/i386elfgcc`，因此
现在让我们导出一些路径。请根据您的喜好随意修改：

```shell
export PREFIX="/usr/local/i386elfgcc"
export TARGET=i386-elf
export PATH="$PREFIX/bin:$PATH"
```


## binutils

记住：在粘贴互联网上的文本之前一定要小心。我建议一行一行地抄写。

```shell
mkdir /tmp/src
cd /tmp/src
curl -O http://ftp.gnu.org/gnu/binutils/binutils-2.24.tar.gz # If the link 404's, look for a more recent version
tar xf binutils-2.24.tar.gz
mkdir binutils-build
cd binutils-build
../binutils-2.24/configure --target=$TARGET --enable-interwork --enable-multilib --disable-nls --disable-werror --prefix=$PREFIX 2>&1 | tee configure.log
make all install 2>&1 | tee make.log
```


## gcc

```
cd /tmp/src
curl -O https://ftp.gnu.org/gnu/gcc/gcc-4.9.1/gcc-4.9.1.tar.bz2
tar xf gcc-4.9.1.tar.bz2
mkdir gcc-build
cd gcc-build
../gcc-4.9.1/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-libssp --enable-languages=c --without-headers
make all-gcc 
make all-target-libgcc 
make install-gcc 
make install-target-libgcc
```

就是这样！现在你应该可以在`/usr/local/i386elfgcc/bin`目录下看到所有GNU binutils
和编译器，前缀`i386-elf-`是为了避免与系统编译器和binutils产生冲突。

你可能想要将`$PATH`添加到您的`.bashrc`。从现在开始，在本教程中，我们将在使用交叉
编译的gcc时显式地使用前缀。
