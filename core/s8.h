#include "memory.h"
#include "types.h"

#define s8(s) \
    (s8) { (u8 *)s, sizeof(s) - 1 }
typedef struct {
    u8 *str;
    size_t len;
} s8;

s8 s8_new(arena *a, size_t len);
s8 s8_from_cstr(arena *a, char *str);
s8 s8_cpy(arena *a, s8 str);

s8 s8_cat(arena *a, s8 str1, s8 str2);
