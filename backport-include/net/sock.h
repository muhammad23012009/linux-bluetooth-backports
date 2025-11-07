#ifndef __BACKPORT_NET_SOCK_H
#define __BACKPORT_NET_SOCK_H
#include_next <net/sock.h>
#include <linux/version.h>

#if LINUX_VERSION_IS_LESS(4,16,0)
#define sk_pacing_shift_update LINUX_BACKPORT(sk_pacing_shift_update)
static inline void sk_pacing_shift_update(struct sock *sk, int val)
{
#if LINUX_VERSION_IS_GEQ(4,15,0)
	if (!sk || !sk_fullsock(sk) || sk->sk_pacing_shift == val)
		return;
	sk->sk_pacing_shift = val;
#endif /* >= 4.15 */
}
#endif /* < 4.16 */

#if LINUX_VERSION_IS_LESS(5,14,0)
static inline void backport_sk_error_report(struct sock *sk)
{
	sk->sk_error_report(sk);
}
#define sk_error_report(sk) LINUX_BACKPORT(sk_error_report(sk))
#endif /* <= 5.14 */

#if LINUX_VERSION_IS_LESS(6,4,0)
void __sock_recv_cmsgs(struct msghdr *msg, struct sock *sk,
                       struct sk_buff *skb);

#define SK_DEFAULT_STAMP (-1L * NSEC_PER_SEC)
static inline void sock_recv_cmsgs(struct msghdr *msg, struct sock *sk,
                                   struct sk_buff *skb)
{
#define SOCK_RCVMARK 27 /* Hardcoded according to ABI */
#define FLAGS_RECV_CMSGS ((1UL << SOCK_RXQ_OVFL)                        | \
                           (1UL << SOCK_RCVTSTAMP)                      | \
                           (1UL << SOCK_RCVMARK))
#define TSFLAGS_ANY       (SOF_TIMESTAMPING_SOFTWARE                    | \
                           SOF_TIMESTAMPING_RAW_HARDWARE)

        if (sk->sk_flags & FLAGS_RECV_CMSGS ||
            READ_ONCE(sk->sk_tsflags) & TSFLAGS_ANY)
                __sock_recv_cmsgs(msg, sk, skb);
        else if (unlikely(sock_flag(sk, SOCK_TIMESTAMP)))
                sock_write_timestamp(sk, skb->tstamp);
        else if (unlikely(sock_read_timestamp(sk) == SK_DEFAULT_STAMP))
                sock_write_timestamp(sk, 0);
}
 
#endif /* <= 6.4.0 */ 

#endif /* __BACKPORT_NET_SOCK_H */
