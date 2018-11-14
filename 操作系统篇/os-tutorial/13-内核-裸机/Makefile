# $@ = 目标文件
# $< = 第一个依赖
# $^ = 全部依赖

# 第一个规则是在没有向Makefile提供参数时执行的
all: run

# 注意如何根据需要构建依赖
kernel.bin: kernel_entry.o kernel.o
	i386-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

kernel_entry.o: kernel_entry.asm
	nasm $< -f elf -o $@

kernel.o: kernel.c
	i386-elf-gcc -ffreestanding -c $< -o $@

# 用于对内核进行反汇编的规则 - 可能对调试有用
kernel.dis: kernel.bin
	ndisasm -b 32 $< > $@

bootsect.bin: bootsect.asm
	nasm $< -f bin -o $@

os-image.bin: bootsect.bin kernel.bin
	cat $^ > $@

run: os-image.bin
	qemu-system-i386 -fda $< --curses

clean:
	rm *.bin *.o *.dis
