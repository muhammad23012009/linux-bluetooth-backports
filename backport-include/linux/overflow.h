#ifndef __BACKPORT_LINUX_OVERFLOW
#define __BACKPORT_LINUX_OVERFLOW
#include <linux/version.h>
#include_next <linux/overflow.h>

/* TO FIX */
#if LINUX_VERSION_IS_LESS(6,3,0)
#define struct_size_t(type, member, count)                                      \
        struct_size((type *)NULL, member, count)

#define __DEFINE_FLEX(type, name, member, count, trailer...)                    \
        _Static_assert(__builtin_constant_p(count),                             \
                       "onstack flex array members require compile-time const count"); \
        union {                                                                 \
                u8 bytes[struct_size_t(type, member, count)];                   \
                type obj;                                                       \
        } name##_u trailer;                                                     \
        type *name = (type *)&name##_u

#define _DEFINE_FLEX(type, name, member, count, initializer...)                 \
        __DEFINE_FLEX(type, name, member, count, = { .obj initializer })

#define DEFINE_FLEX(TYPE, NAME, MEMBER, COUNTER, COUNT) \
        _DEFINE_FLEX(TYPE, NAME, MEMBER, COUNT, = { .COUNTER = COUNT, })

#define DEFINE_RAW_FLEX(type, name, member, count)      \
        __DEFINE_FLEX(type, name, member, count, = { })

#endif

#endif
