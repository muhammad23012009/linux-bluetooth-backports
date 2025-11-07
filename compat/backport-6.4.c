// SPDX-License-Identifier: GPL-2.0

#include <linux/export.h>
#include <linux/usb.h>
#include <net/sock.h>

#if LINUX_VERSION_IS_LESS(6,3,5)

#if LINUX_VERSION_IS_LESS(5,4,244)
/* TODO: Maybe move this to backport-5.15? Also fix required version */

/**
 * usb_find_endpoint() - Given an endpoint address, search for the endpoint's
 * usb_host_endpoint structure in an interface's current altsetting.
 * @intf: the interface whose current altsetting should be searched
 * @ep_addr: the endpoint address (number and direction) to find
 *
 * Search the altsetting's list of endpoints for one with the specified address.
 *
 * Return: Pointer to the usb_host_endpoint if found, %NULL otherwise.
 */
static const struct usb_host_endpoint *usb_find_endpoint(
		const struct usb_interface *intf, unsigned int ep_addr)
{
	int n;
	const struct usb_host_endpoint *ep;

	n = intf->cur_altsetting->desc.bNumEndpoints;
	ep = intf->cur_altsetting->endpoint;
	for (; n > 0; (--n, ++ep)) {
		if (ep->desc.bEndpointAddress == ep_addr)
			return ep;
	}
	return NULL;
}

/**
 * usb_check_bulk_endpoints - Check whether an interface's current altsetting
 * contains a set of bulk endpoints with the given addresses.
 * @intf: the interface whose current altsetting should be searched
 * @ep_addrs: 0-terminated array of the endpoint addresses (number and
 * direction) to look for
 *
 * Search for endpoints with the specified addresses and check their types.
 *
 * Return: %true if all the endpoints are found and are bulk, %false otherwise.
 */
bool usb_check_bulk_endpoints(
		const struct usb_interface *intf, const u8 *ep_addrs)
{
	const struct usb_host_endpoint *ep;

	for (; *ep_addrs; ++ep_addrs) {
		ep = usb_find_endpoint(intf, *ep_addrs);
		if (!ep || !usb_endpoint_xfer_bulk(&ep->desc))
			return false;
	}
	return true;
}
EXPORT_SYMBOL_GPL(usb_check_bulk_endpoints);

/**
 * usb_check_int_endpoints - Check whether an interface's current altsetting
 * contains a set of interrupt endpoints with the given addresses.
 * @intf: the interface whose current altsetting should be searched
 * @ep_addrs: 0-terminated array of the endpoint addresses (number and
 * direction) to look for
 *
 * Search for endpoints with the specified addresses and check their types.
 *
 * Return: %true if all the endpoints are found and are interrupt,
 * %false otherwise.
 */
bool usb_check_int_endpoints(
		const struct usb_interface *intf, const u8 *ep_addrs)
{
	const struct usb_host_endpoint *ep;

	for (; *ep_addrs; ++ep_addrs) {
		ep = usb_find_endpoint(intf, *ep_addrs);
		if (!ep || !usb_endpoint_xfer_int(&ep->desc))
			return false;
	}
	return true;
}
EXPORT_SYMBOL_GPL(usb_check_int_endpoints);
#endif

static inline void sock_recv_drops(struct msghdr *msg, struct sock *sk,
                                   struct sk_buff *skb)
{
        if (sock_flag(sk, SOCK_RXQ_OVFL) && skb && SOCK_SKB_CB(skb)->dropcount)
                put_cmsg(msg, SOL_SOCKET, SO_RXQ_OVFL,
                        sizeof(__u32), &SOCK_SKB_CB(skb)->dropcount);
}
 
static void sock_recv_mark(struct msghdr *msg, struct sock *sk,
                           struct sk_buff *skb)
{
        if (sock_flag(sk, SOCK_RCVMARK) && skb) {
                /* We must use a bounce buffer for CONFIG_HARDENED_USERCOPY=y */
                __u32 mark = skb->mark;

                put_cmsg(msg, SOL_SOCKET, SO_MARK, sizeof(__u32), &mark);
        }
}

void __sock_recv_cmsgs(struct msghdr *msg, struct sock *sk,
                       struct sk_buff *skb)
{
        sock_recv_timestamp(msg, sk, skb);
        sock_recv_drops(msg, sk, skb);
        sock_recv_mark(msg, sk, skb);
}
EXPORT_SYMBOL_GPL(__sock_recv_cmsgs);

#if LINUX_VERSION_IS_LESS(6,1,0)
u32 __get_random_u32_below(u32 ceil)
{
        /*
         * This is the slow path for variable ceil. It is still fast, most of
         * the time, by doing traditional reciprocal multiplication and
         * opportunistically comparing the lower half to ceil itself, before
         * falling back to computing a larger bound, and then rejecting samples
         * whose lower half would indicate a range indivisible by ceil. The use
         * of `-ceil % ceil` is analogous to `2^32 % ceil`, but is computable
         * in 32-bits.
         */
        u32 rand = get_random_u32();
        u64 mult;

        /*
         * This function is technically undefined for ceil == 0, and in fact
         * for the non-underscored constant version in the header, we build bug
         * on that. But for the non-constant case, it's convenient to have that
         * evaluate to being a straight call to get_random_u32(), so that
         * get_random_u32_inclusive() can work over its whole range without
         * undefined behavior.
         */
        if (unlikely(!ceil))
                return rand;

        mult = (u64)ceil * rand;
        if (unlikely((u32)mult < ceil)) {
                u32 bound = -ceil % ceil;
                while (unlikely((u32)mult < bound))
                        mult = (u64)ceil * get_random_u32();
        }
        return mult >> 32;
}
EXPORT_SYMBOL_GPL(__get_random_u32_below);
#endif

#endif
