#ifndef __BACKPORT_RANDOM_H
#define __BACKPORT_RANDOM_H
#include_next <linux/random.h>
#include <linux/version.h>

#if LINUX_VERSION_IS_LESS(6,1,0)
static inline u8 get_random_u8(void)
{
	return get_random_u32() & 0xff;
}

static inline u16 get_random_u16(void)
{
	return get_random_u32() & 0xffff;
}

u32 __get_random_u32_below(u32 ceil);

static inline u32 get_random_u32_below(u32 ceil)
{
        if (!__builtin_constant_p(ceil))
                return __get_random_u32_below(ceil);

        /*
         * For the fast path, below, all operations on ceil are precomputed by
         * the compiler, so this incurs no overhead for checking pow2, doing
         * divisions, or branching based on integer size. The resultant
         * algorithm does traditional reciprocal multiplication (typically
         * optimized by the compiler into shifts and adds), rejecting samples
         * whose lower half would indicate a range indivisible by ceil.
         */
        BUILD_BUG_ON_MSG(!ceil, "get_random_u32_below() must take ceil > 0");
        if (ceil <= 1)
                return 0;
        for (;;) {
                if (ceil <= 1U << 8) {
                        u32 mult = ceil * get_random_u8();
                        if (likely(is_power_of_2(ceil) || (u8)mult >= (1U << 8) % ceil))
                                return mult >> 8;
                } else if (ceil <= 1U << 16) {
                        u32 mult = ceil * get_random_u16();
                        if (likely(is_power_of_2(ceil) || (u16)mult >= (1U << 16) % ceil))
                                return mult >> 16;
                } else {
                        u64 mult = (u64)ceil * get_random_u32();
                        if (likely(is_power_of_2(ceil) || (u32)mult >= -ceil % ceil))
                                return mult >> 32;
                }
        }
}

static inline u32 get_random_u32_inclusive(u32 floor, u32 ceil)
{
        BUILD_BUG_ON_MSG(__builtin_constant_p(floor) && __builtin_constant_p(ceil) &&
                         (floor > ceil || ceil - floor == U32_MAX),
                         "get_random_u32_inclusive() must take floor <= ceil");
        return floor + get_random_u32_below(ceil - floor + 1);
}

#endif

#endif /* __BACKPORT_RANDOM_H */
