[bits 32]
[extern main] ; 定义调用点,必须与kernel.c中‘main’函数具有相同的名字
call main     ; 调用C函数,链接器将知道它在内存中的位置
jmp $
