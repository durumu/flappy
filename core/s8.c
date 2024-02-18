#include "s8.h"

#include <string.h>

s8 s8_new(arena *a, size_t len) {
    s8 s = {0};
    s.len = len;
    s.str = alloc(a, u8, len + 1);
    s.str[len] = 0;  // this should be redundant, but just in case
    return s;
}

s8 s8_from_cstr(arena *a, char *str) {
    s8 s = {0};
    s.len = strlen(str);
    s.str = alloc(a, u8, s.len + 1, NOZERO);
    memcpy(s.str, str, s.len + 1);
    return s;
}

s8 s8_cpy(arena *a, s8 str) {
    s8 s = {0};
    s.len = str.len;
    s.str = alloc(a, u8, s.len + 1, NOZERO);
    s.str[s.len] = 0;
    memcpy(s.str, str.str, s.len);
    return s;
}

s8 s8_cat(arena *a, s8 str1, s8 str2) {
    s8 s = {0};
    s.len = str1.len + str2.len;
    s.str = alloc(a, u8, s.len + 1, NOZERO);
    s.str[s.len] = 0;
    memcpy(s.str, str1.str, str1.len);
    memcpy(s.str + str1.len, str2.str, str2.len);
    return s;
}
