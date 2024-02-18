#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

#define sizeof(x) (size)sizeof(x)
#define alignof(x) (size) _Alignof(x)
#define countof(a) (sizeof(a) / sizeof(*(a)))

typedef struct {
    char *begin;
    char *end;
} arena;

u32 NOZERO = 1 << 0;
u32 SOFTFAIL = 1 << 1;

__attribute((malloc, alloc_size(2, 4), alloc_align(3))) void *alloc_internal(
    arena *a, size sz, size align, size count, u32 flags) {
    size padding = -(uptr)a->begin & (align - 1);
    size available = a->end - a->begin - padding;
    if (available < 0 || count * sz > available) {
        if (flags & SOFTFAIL) {
            return NULL;
        }
        abort();  // one possible out-of-memory policy
    }
    void *p = a->begin + padding;
    a->begin += padding + count * sz;
    return (flags & NOZERO) ? p : memset(p, 0, count * sz);
}

#define alloc(...) \
    allocx(__VA_ARGS__, alloc4, alloc3, alloc2, alloc1, alloc0)(__VA_ARGS__)
#define allocx(a, b, c, d, e, ...) e
#define alloc0() _Static_assert(0, "alloc requires at least 2 arguments")
#define alloc1(a) _Static_assert(0, "alloc requires at least 2 arguments")
#define alloc2(a, type) \
    (type *)alloc_internal(a, sizeof(type), alignof(type), count, 0)
#define alloc3(a, type, count) \
    (type *)alloc_internal(a, sizeof(type), alignof(type), count, 0)
#define alloc4(a, type, count, flags) \
    (type *)alloc_internal(a, sizeof(type), alignof(type), count, flags)

arena arena_init(size size) {
    arena a = {0};
    a.begin = (char *)malloc(size);
    a.end = a.begin + size;
    return a;
}

void arena_free(arena *a) {
    free(a->begin);
    a->begin = a->end = NULL;
}
