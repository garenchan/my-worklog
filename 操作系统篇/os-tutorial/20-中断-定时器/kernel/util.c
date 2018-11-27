#include "util.h"

void memory_copy(char *source, char *dest, int nbytes) {
    int i;
    for (i = 0; i < nbytes; ++i) {
        *(dest + i) = *(source + i);
    }
}

void memory_set(u8 *dest, u8 val, u32 len) {
    for ( ; len != 0; --len) *dest++ = val;
}

/* K&R */
void int_to_ascii(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) n *= -1;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    
    if (sign < 0) str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

/* K&R */
void reverse(char s[]) {
    if (s == 0) return;
    int c, i, j;
    for (i = 0, j = strlen(s) - 1; i < j; ++i, --j) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
     }
}

/* K&R */
int strlen(char s[]) {
    int i = 0;
    if (s == 0) return i;
    while (s[i] != '\0') ++i;
    return i;
}
