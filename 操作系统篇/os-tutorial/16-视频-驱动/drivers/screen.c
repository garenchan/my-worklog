#include "screen.h"
#include "ports.h"

/* 声明私有函数 */
int get_cursor_offset();
void set_cursor_offset(int offset);
int print_char(char c, int col, int row, char attr);
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);

/**********************************************
 * 公共内核API函数                            *
 **********************************************/

/**
 * 在指定位置打印消息
 * 如果行和列为负数，我们将使用当前偏移量
 */
void kprint_at(char *message, int col, int row) {
    int offset;
    if (col >= 0 && row >= 0) {
        offset = get_offset(col, row);
    } else {
        offset = get_cursor_offset();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

    /* 循环并打印消息 */
    int i = 0;
    while (message[i] != '\0') {
        offset = print_char(message[i++], col, row, WHITE_ON_BLACK);
        /* 计算下一次迭代的 row/col */
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }
}

void kprint(char *message) {
    kprint_at(message, -1, -1);
}

void clear_screen() {
    int screen_size = MAX_COLS * MAX_ROWS;
    int i;
    char *screen = VIDEO_ADDRESS;

    for (i = 0; i < screen_size; ++i) {
        screen[i*2] = ' ';
        screen[i*2+1] = WHITE_ON_BLACK;
    }
    set_cursor_offset(get_offset(0, 0));
}

/**********************************************
 * 私有内核函数                               *
 **********************************************/

/**
 * 我们内核最底层的打印函数，直接访问显存
 * 
 * 如果col和row为负数，我们将在当前光标位置打印；
 * 如果attr为0，那么默认将使用“黑底白字”；
 * 返回下一个字符的偏移量；
 * 设置视频光标位置到返回的偏移量
 */
int print_char(char c, int col, int row, char attr) {
    unsigned char *vidmem = (unsigned char *)VIDEO_ADDRESS;
    if (!attr) attr = WHITE_ON_BLACK;

    /* 差错控制：如果坐标不正确那么打印一个红色的‘E’ */
    if (col >= MAX_COLS || row >= MAX_ROWS) {
        vidmem[2 * MAX_COLS * MAX_ROWS - 2] = 'E';
        vidmem[2 * MAX_COLS * MAX_ROWS - 1] = RED_ON_WHITE;
        return get_offset(col, row);
    }

    int offset;
    if (col >= 0 && row >= 0) offset = get_offset(col, row);
    else offset = get_cursor_offset();

    if (c == '\n') {
        row = get_offset_row(offset);
        offset = get_offset(0, row + 1);
    } else {
        vidmem[offset] = c;
        vidmem[offset + 1] = attr;
        offset += 2;
    }
    set_cursor_offset(offset);
    return offset;
}

int get_cursor_offset() {
    /* 使用VGA端口来获取当前光标位置
     * 1. 请求偏移量的高字节 (数据 14)
     * 2. 请求偏移量的低字节 (数据 15)
     */
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8; /* 高字节左移8位 */
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset * 2; /* 位置 * 字符格大小 */
}

void set_cursor_offset(int offset) {
    /* 类似get_cursor_offset，只是这次是写数据 */
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

int get_offset(int col, int row) { return 2 * (row * MAX_COLS + col); }
int get_offset_row(int offset) { return offset / (2 * MAX_COLS); }
int get_offset_col(int offset) { return (offset - get_offset_row(offset) * 2 * MAX_COLS) / 2; }
