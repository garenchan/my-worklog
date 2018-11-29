; 在isr.c中定义的
[extern isr_handler]
[extern irq_handler]

; 通用ISR代码
isr_common_stub:
    ; 1. 保存CPU状态
    pusha ; edi, esi, ebp, esp, ebx, edx, ecx, eax入栈
    mov ax, ds ; eax的低16位 = ds
    push eax ; 保存数据段描述符
    mov ax, 0x10; 内核数据段描述符
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push esp ; registers_t *r

    ; 2. 调用C处理器
    cld ; 遵循sysV ABI的C代码要求在函数入口处将DF标志位清零
    call isr_handler

    ; 3. 恢复状态
    pop eax
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa
    add esp, 8 ; 清除已推送的错误代码和ISR号
    iret ; 同时弹出5个东西：CS, EIP, EFLAGS, SS和ESP

; 通用IRQ代码，与ISR代码基本相同，除了“call”调用的处理例程不同以及“pop ebx”
irq_common_stub:
    pusha
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push esp
    cld
    call irq_handler ; 和ISR代码不同的地方之一
    pop ebx          ; 和ISR代码不同的地方之二
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    popa
    add esp, 8
    iret

; 我们不知道哪个中断是调用者
; 当处理程序运行时，我们需要为每个中断使用不同的处理程序。
; 此外，一些中断会将错误代码推送到堆栈上，而另一些中断则不会，因此，我们将为那些
; 不会的中断推送一个虚拟的错误代码，这样我们就有了一个一致的栈。

; 首先让这些IRSs全局化
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31
; IRQs
global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

; 0: 除0异常
isr0:
    push byte 0
    push byte 0
    jmp isr_common_stub

; 1: 调试异常
isr1:
    push byte 0
    push byte 1
    jmp isr_common_stub

; 2: 非可屏蔽中断异常
isr2:
    push byte 0
    push byte 2
    jmp isr_common_stub

; 3: Int 3异常
isr3:
    push byte 0
    push byte 3
    jmp isr_common_stub

; 4: INTO异常
isr4:
    push byte 0
    push byte 4
    jmp isr_common_stub

; 5: 越界异常
isr5:
    push byte 0
    push byte 5
    jmp isr_common_stub

; 6: 无效指令异常
isr6:
    push byte 0
    push byte 6
    jmp isr_common_stub

; 7: 协处理器不可用异常
isr7:
    push byte 0
    push byte 7
    jmp isr_common_stub

; 8: 双精度错误异常（带错误代码）
isr8:
    push byte 8
    jmp isr_common_stub

; 9: 协处理器段溢出异常
isr9:
    push byte 0
    push byte 9
    jmp isr_common_stub

; 10: 坏的TSS异常（带错误代码）
isr10:
    push byte 10
    jmp isr_common_stub

; 11: 段不存在异常（带错误代码）
isr11:
    push byte 11
    jmp isr_common_stub

; 12: 栈错误异常（带错误代码）
isr12:
    push byte 12
    jmp isr_common_stub

; 13: 一般保护故障异常（带错误代码）
isr13:
    push byte 13
    jmp isr_common_stub

; 14: 页错误异常（带错误代码）
isr14:
    push byte 14
    jmp isr_common_stub

; 15: 保留异常
isr15:
    push byte 0
    push byte 15
    jmp isr_common_stub

; 16: 浮点格式异常
isr16:
    push byte 0
    push byte 16
    jmp isr_common_stub

; 17: 对齐检查异常
isr17:
    push byte 0
    push byte 17
    jmp isr_common_stub

; 18: 机器检查异常
isr18:
    push byte 0
    push byte 18
    jmp isr_common_stub

; 19: 保留
isr19:
    push byte 0
    push byte 19
    jmp isr_common_stub

; 20: 保留
isr20
    push byte 0
    push byte 20
    jmp isr_common_stub

; 21: 保留
isr21
    push byte 0
    push byte 21
    jmp isr_common_stub

; 22: 保留
isr22
    push byte 0
    push byte 22
    jmp isr_common_stub

; 23: 保留
isr23
    push byte 0
    push byte 23
    jmp isr_common_stub

; 24: 保留
isr24
    push byte 0
    push byte 24
    jmp isr_common_stub

; 25: 保留
isr25
    push byte 0
    push byte 25
    jmp isr_common_stub

; 26: 保留
isr26
    push byte 0
    push byte 26
    jmp isr_common_stub

; 27: 保留
isr27
    push byte 0
    push byte 27
    jmp isr_common_stub

; 28: 保留
isr28
    push byte 0
    push byte 28
    jmp isr_common_stub

; 29: 保留
isr29
    push byte 0
    push byte 29
    jmp isr_common_stub

; 30: 保留
isr30
    push byte 0
    push byte 30
    jmp isr_common_stub

; 31: 保留
isr31
    push byte 0
    push byte 31
    jmp isr_common_stub

; IRQ处理程序
irq0:
    push byte 0
    push byte 32
    jmp irq_common_stub

irq1:
    push byte 1
    push byte 33
    jmp irq_common_stub

irq2:
    push byte 2
    push byte 34
    jmp irq_common_stub

irq3:
    push byte 3
    push byte 35
    jmp irq_common_stub

irq4:
    push byte 4
    push byte 36
    jmp irq_common_stub

irq5:
    push byte 5
    push byte 37
    jmp irq_common_stub

irq6:
    push byte 6
    push byte 38
    jmp irq_common_stub

irq7:
    push byte 7
    push byte 39
    jmp irq_common_stub

irq8:
    push byte 8
    push byte 40
    jmp irq_common_stub

irq9:
    push byte 9
    push byte 41
    jmp irq_common_stub

irq10:
    push byte 10
    push byte 42
    jmp irq_common_stub

irq11:
    push byte 11
    push byte 43
    jmp irq_common_stub

irq12:
    push byte 12
    push byte 44
    jmp irq_common_stub

irq13:
    push byte 13
    push byte 45
    jmp irq_common_stub

irq14:
    push byte 14
    push byte 46
    jmp irq_common_stub

irq15:
    push byte 15
    push byte 47
    jmp irq_common_stub
