#include "../drivers/ports.h"

void main() {
    /* 屏幕光标位置：向VGA控制寄存器 (0x3d4) 查询位置字节
     * 14 = 光标位置高字节，15 = 光标位置低字节。*/
    port_byte_out(0x3d4, 14); // 请求字节14:光标位置的高字节
    /* 数据在VGA数据寄存器 (0x3d5) 中返回 */
    int position = port_byte_in(0x3d5);
    position = position << 8; // 高字节

    port_byte_out(0x3d4, 15); // 请求低字节
    position += port_byte_in(0x3d5);

    /* VGA"单元格"由字符及其控制数据组成
     * 例如，'黑底白字'， '白底红字'等 */
    int offset_from_vga = position * 2;

    /* 现在你可以使用gdb检查这两个变量，因为我们仍然不知道如
     * 何在屏幕上打印字符串。
     * 运行'make debug'然后在gdb控制台中执行如下命令：
     *     b kernel.c:21
     *     continue
     *     print position
     *     print offset_from_vga
     */

    /* 让我们写在当前光标位置上，我们已经知道怎么做了 */
    char *vga = 0xb8000;
    vga[offset_from_vga] = 'X';
    vga[offset_from_vga + 1] = 0x0f; // 黑底白字
}
