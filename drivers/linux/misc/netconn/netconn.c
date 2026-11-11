#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <net/tcp.h>
#include <net/tcp_states.h>
#include <net/udp.h>
#include <net/net_namespace.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 13, 0)
#error version not supported
#endif

static void
addr_port_show(struct seq_file *seq, sa_family_t family, const void *addr, __u16 port)
{
	seq_setwidth(seq, 23);
	seq_printf(seq, family == AF_INET6 ? "%pI6c" : "%pI4", addr);
	if (port == 0)
		seq_puts(seq, ":*");
	else
		seq_printf(seq, ":%d", port);
	seq_pad(seq, ' ');
}

static int
tcp_seq_show(struct seq_file *seq, void *v)
{
	struct tcp_iter_state *st;
	struct sock *sk;
	sa_family_t family;
	const void *dst, *src;
	__u16 dstp, srcp;

	if (v == SEQ_START_TOKEN) {
		seq_printf(seq, "Local Addr              Remote Addr\n");
		return 0;
	}

	st = seq->private;
	sk = v;
	family = st->family;
	switch (st->state) {
	case TCP_SEQ_STATE_LISTENING:
	case TCP_SEQ_STATE_ESTABLISHED:
		if (sk->sk_state == TCP_TIME_WAIT) {
			const struct inet_timewait_sock *tw = v;

			if (family == AF_INET6) {
				dst = &tw->tw_v6_daddr;
				src = &tw->tw_v6_rcv_saddr;
			} else {
				dst = &tw->tw_daddr;
				src = &tw->tw_rcv_saddr;
			}
			dstp = ntohs(tw->tw_dport);
			srcp = ntohs(tw->tw_sport);
		} else {
			const struct tcp_sock *tp;
			const struct inet_sock *inet;

			tp = tcp_sk(sk);
			inet = inet_sk(sk);

			if (family == AF_INET6) {
				dst = &sk->sk_v6_daddr;
				src = &sk->sk_v6_rcv_saddr;
			} else {
				dst = &inet->inet_daddr;
				src = &inet->inet_rcv_saddr;
			}
			dstp = ntohs(inet->inet_dport);
			srcp = ntohs(inet->inet_sport);
		}
		break;

	default:
		return 0;
	}

	addr_port_show(seq, family, src, srcp);
	addr_port_show(seq, family, dst, dstp);
	seq_printf(seq, "\n");
	return 0;
}

static int
udp_seq_show(struct seq_file *seq, void *v)
{
	struct udp_iter_state *st;
	sa_family_t family;
	struct sock *sk;
	struct inet_sock *inet;
	const void *dst, *src;
	__u16 dstp, srcp;

	if (v == SEQ_START_TOKEN) {
		seq_printf(seq, "Local Addr              Remote Addr\n");
		return 0;
	}

	st = seq->private;
	sk = v;
	family = st->family;
	inet = inet_sk(sk);
	if (family == AF_INET6) {
		dst = &sk->sk_v6_daddr;
		src = &sk->sk_v6_rcv_saddr;
	} else {
		dst = &inet->inet_daddr;
		src = &inet->inet_rcv_saddr;
	}
	dstp = ntohs(inet->inet_dport);
	srcp = ntohs(inet->inet_sport);

	addr_port_show(seq, family, src, srcp);
	addr_port_show(seq, family, dst, dstp);
	seq_printf(seq, "\n");
	return 0;
}

static const struct file_operations tcp_afinfo_seq_fops = {
    .owner = THIS_MODULE,
    .open = tcp_seq_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release_net,
};

static struct tcp_seq_afinfo tcp4_seq_afinfo = {
    .name = "tcpcon",
    .family = AF_INET,
    .seq_fops = &tcp_afinfo_seq_fops,
    .seq_ops = {
        .show = tcp_seq_show,
    },
};

static struct tcp_seq_afinfo tcp6_seq_afinfo = {
    .name = "tcp6con",
    .family = AF_INET6,
    .seq_fops = &tcp_afinfo_seq_fops,
    .seq_ops = {
        .show = tcp_seq_show,
    },
};

static const struct file_operations udp_afinfo_seq_fops = {
    .owner = THIS_MODULE,
    .open = udp_seq_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release_net,
};

static struct udp_seq_afinfo udp4_seq_afinfo = {
    .name = "udpcon",
    .family = AF_INET,
    .udp_table = &udp_table,
    .seq_fops = &udp_afinfo_seq_fops,
    .seq_ops = {
        .show = udp_seq_show,
    },
};

static struct udp_seq_afinfo udp6_seq_afinfo = {
    .name = "udp6con",
    .family = AF_INET6,
    .udp_table = &udp_table,
    .seq_fops = &udp_afinfo_seq_fops,
    .seq_ops = {
        .show = udp_seq_show,
    },
};

static int __net_init
nt_init(struct net *net)
{
	int rv, n;

	n = 0;
	rv = tcp_proc_register(net, &tcp4_seq_afinfo);
	if (rv < 0)
		goto cleanup;
	n++;

	rv = tcp_proc_register(net, &tcp6_seq_afinfo);
	if (rv < 0)
		goto cleanup;
	n++;

	rv = udp_proc_register(net, &udp4_seq_afinfo);
	if (rv < 0)
		goto cleanup;
	n++;

	rv = udp_proc_register(net, &udp6_seq_afinfo);
	if (rv < 0)
		goto cleanup;

	return rv;

cleanup:
	if (n > 2)
		udp_proc_unregister(net, &udp4_seq_afinfo);
	if (n > 1)
		tcp_proc_unregister(net, &tcp6_seq_afinfo);
	if (n > 0)
		tcp_proc_unregister(net, &tcp4_seq_afinfo);
	return rv;
}

static void __net_exit
nt_exit(struct net *net)
{
	tcp_proc_unregister(net, &tcp4_seq_afinfo);
	tcp_proc_unregister(net, &tcp6_seq_afinfo);
	udp_proc_unregister(net, &udp4_seq_afinfo);
	udp_proc_unregister(net, &udp6_seq_afinfo);
}

static struct pernet_operations nt_ops = {
    .init = nt_init,
    .exit = nt_exit,
};

static __init int
nt_mod_init(void)
{
	pr_info("netconn: init\n");
	return register_pernet_subsys(&nt_ops);
}

static void
nt_mod_exit(void)
{
	pr_info("netconn: exit\n");
	unregister_pernet_subsys(&nt_ops);
}

module_init(nt_mod_init);
module_exit(nt_mod_exit);

MODULE_AUTHOR("Quan Tran");
MODULE_DESCRIPTION("Display network connections");
MODULE_LICENSE("GPL");
