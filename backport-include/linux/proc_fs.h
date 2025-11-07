#ifndef __BACKPORT_LINUX_PROC_FS
#define __BACKPORT_LINUX_PROC_FS
#include_next <linux/proc_fs.h>

static inline void *pde_data(const struct inode *inode)
{
	return inode->i_private;
}

#endif
