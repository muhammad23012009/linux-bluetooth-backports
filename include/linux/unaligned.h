/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_UNALIGNED_H
#define __LINUX_UNALIGNED_H

/*
 * This is the most generic implementation of unaligned accesses
 * and should work almost anywhere.
 */
#include <linux/unaligned/packed_struct.h>
#include <asm/byteorder.h>
#include <vdso/unaligned.h>

static inline u32 __get_unaligned_le24(const u8 *p)
{
	return p[0] | p[1] << 8 | p[2] << 16;
}

static inline u32 get_unaligned_le24(const void *p)
{
	return __get_unaligned_le24(p);
}

static inline void __put_unaligned_be24(const u32 val, u8 *p)
{
	*p++ = (val >> 16) & 0xff;
	*p++ = (val >> 8) & 0xff;
	*p++ = val & 0xff;
}

static inline void put_unaligned_be24(const u32 val, void *p)
{
	__put_unaligned_be24(val, p);
}

static inline void __put_unaligned_le24(const u32 val, u8 *p)
{
	*p++ = val & 0xff;
	*p++ = (val >> 8) & 0xff;
	*p++ = (val >> 16) & 0xff;
}

static inline void put_unaligned_le24(const u32 val, void *p)
{
	__put_unaligned_le24(val, p);
}

static inline void __put_unaligned_be48(const u64 val, u8 *p)
{
	*p++ = (val >> 40) & 0xff;
	*p++ = (val >> 32) & 0xff;
	*p++ = (val >> 24) & 0xff;
	*p++ = (val >> 16) & 0xff;
	*p++ = (val >> 8) & 0xff;
	*p++ = val & 0xff;
}

static inline void put_unaligned_be48(const u64 val, void *p)
{
	__put_unaligned_be48(val, p);
}

static inline u64 __get_unaligned_be48(const u8 *p)
{
	return (u64)p[0] << 40 | (u64)p[1] << 32 | (u64)p[2] << 24 |
		p[3] << 16 | p[4] << 8 | p[5];
}

static inline u64 get_unaligned_be48(const void *p)
{
	return __get_unaligned_be48(p);
}

#endif /* __LINUX_UNALIGNED_H */
