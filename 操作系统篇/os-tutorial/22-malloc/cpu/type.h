#ifndef TYPE_H
#define TYPE_H

/* 与其使用“字符”来分配非字符字节，我们将使用这些没有语义的新类型 */
typedef unsigned int   u32;
typedef          int   s32;
typedef unsigned short u16;
typedef          short s16;
typedef unsigned char  u8;
typedef          char  s8;

#define low_16(address) (u16)((address) & 0xFFFF)
#define high_16(address) (u16)(((address) >> 16) & 0xFFFF)

#endif
