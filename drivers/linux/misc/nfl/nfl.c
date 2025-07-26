#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <asm/uaccess.h>

static struct nf_hook_ops nfho;

unsigned int
nfl_log(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	struct iphdr *iph;

	iph = (struct iphdr *)skb_network_header(skb);
	pr_info("nfl: got packet: protocol %d\n", iph->protocol);

	return NF_ACCEPT;
}

static struct nf_hook_ops nfho = {
    .hook = nfl_log,
    .hooknum = 0,
    .pf = PF_INET,
    .priority = NF_IP_PRI_FIRST,
};

static int __init
nfl_init(void)
{
	pr_info("nfl: starting module\n");
	nf_register_net_hook(&init_net, &nfho);
	return 0;
}

static void
nfl_exit(void)
{
	pr_info("nfl: stopping module\n");
	nf_unregister_net_hook(&init_net, &nfho);
}

module_init(nfl_init);
module_exit(nfl_exit);

MODULE_AUTHOR("Quan Tran");
MODULE_DESCRIPTION("IP packet logger");
MODULE_LICENSE("GPL");
