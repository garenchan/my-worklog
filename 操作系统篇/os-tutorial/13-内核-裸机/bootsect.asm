[org 0x7c00]
KERNEL_OFFSET equ 0x1000 ; 和我们在链接内核时使用的相同偏移量

    mov [BOOT_DRIVE], dl ; 请记住BIOS在引导时将'dl'设置为引导驱动器
    mov bp, 0x9000
    mov sp, bp

    mov bx, MSG_REAL_MODE
    call print
    call print_nl

    call load_kernel ; 从磁盘读取内核
    call switch_to_pm ; 关中断,加载GDT,等.最后跳转到'BEGIN_PM'
    jmp $ ; 永远不会被执行

%include "../05-引导扇区-函数和字符串/boot_sect_print.asm"
%include "../05-引导扇区-函数和字符串/boot_sect_print_hex.asm"
%include "../07-引导扇区-磁盘/boot_sect_disk.asm"
%include "../08-32位保护模式-打印/32bit-print.asm"
%include "../09-32位保护模式-GDT/32bit-gdt.asm"
%include "../10-32位保护模式-切换/32bit-switch.asm"

[bits 16]
load_kernel:
    mov bx, MSG_LOAD_KERNEL
    call print
    call print_nl

    mov bx, KERNEL_OFFSET ; 从磁盘读取并存储在0x1000处
    mov dh, 2
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

[bits 32]
BEGIN_PM:
    mov ebx, MSG_PROT_MODE
    call print_string_pm
    call KERNEL_OFFSET ; 将控制权交给内核
    jmp $ ; 当内核将控制权交还给我们时(如果有的话)停留在这里


BOOT_DRIVE db 0 ; 将它存储在内存中是一个好主意,因为“dl”可能会被覆盖
MSG_REAL_MODE db "Started in 16-bit Real Mode", 0
MSG_PROT_MODE db "Landed in 32-bit Protected Mode", 0
MSG_LOAD_KERNEL db "Loading kernel into memory", 0

; padding
times 510 - ($-$$) db 0
dw 0xaa55
