#include "string.h"

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

void append(char s[], char n) {
    int len = strlen(s);
    s[len] = n;
    s[len + 1] = '\0';
}

void backspace(char s[]) {
    int len = strlen(s);
    s[len - 1] = '\0';
}

/* K&R
 * 如果s1 < s2，返回小于0的值；
 * 如果s1 == s2，返回0；
 * 如果s1 > s2，返回值大于0 */
int strcmp(char s1[], char s2[]) {
    int i;
    for (i = 0; s1[i] == s2[i]; ++i) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}
