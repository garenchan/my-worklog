C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h)
# 很好的文件扩展名替换语法
OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o}

# 如果交叉编译器在其他位置，请更改此设置
CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
LD = /usr/local/i386elfgcc/bin/i386-elf-ld
# -g: 在gcc中使用调试符号
CFLAGS = -g

# 第一条规则是默认运行的
os-image.bin: boot/bootsect.bin kernel.bin
	cat $^ > $@

# '--oformat binary'会删除所有符号作为替代品，因此在这种情况下我们不需要手动剥离它们
kernel.bin: boot/kernel_entry.o ${OBJ}
	${LD} -o $@ -Ttext 0x1000 $^ --oformat binary

# 用于调试目的
kernel.elf: boot/kernel_entry.o ${OBJ}
	${LD} -o $@ -Ttext 0x1000 $^

run: os-image.bin
	qemu-system-i386 -fda os-image.bin --curses

# 打开到qemu的连接，并加载我们带符号的内核对象文件
debug: os-image.bin kernel.elf
	qemu-system-i386 -s -S -fda os-image.bin --curses -d guest_errors,int &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# 通配符的通用规则
# 要构建一个对象，总是从它的.c文件进行编译
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o cpu/*.o
