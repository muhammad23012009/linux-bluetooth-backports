#ifndef BACKPORT_UAPI_LINUX_STDDEF
#define BACKPORT_UAPI_LINUX_STDDEF

#include_next <uapi/linux/stddef.h>

#ifndef __counted_by
#define __counted_by(m)
#endif

#ifndef __counted_by_le
#define __counted_by_le(m)
#endif

#ifndef __counted_by_be
#define __counted_by_be(m)
#endif

#endif
