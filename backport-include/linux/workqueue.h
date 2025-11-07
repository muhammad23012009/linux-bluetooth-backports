#ifndef __BACKPORT_LINUX_WORKQUEUE
#define __BACKPORT_LINUX_WORKQUEUE
#include <linux/version.h>
#include_next <linux/workqueue.h>

#if LINUX_VERSION_IS_LESS(6,10,0)
static inline bool disable_work_sync(struct work_struct *work)
{
	return cancel_work_sync(work);
}

static inline bool disable_delayed_work(struct delayed_work *dwork)
{
	return cancel_delayed_work(dwork);
}

static inline bool disable_delayed_work_sync(struct delayed_work *dwork)
{
	return cancel_delayed_work_sync(dwork);
}
#endif /* < 6.10.0 */

#endif
