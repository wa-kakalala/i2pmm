#include <linux/init.h> /* Needed for the macros */
#include <linux/kernel.h> 
#include <linux/module.h> /* Needed by all modules */
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

/**********function define begin**********/
unsigned int nf_out_hook_proc( void * priv, struct sk_buff * skb, 
		        const struct nf_hook_state * state );

unsigned int nf_in_hook_proc( void * priv, struct sk_buff * skb, 
		        const struct nf_hook_state * state );

/**********function define end************/

static struct nf_hook_ops nf_local_out = {
	.hook = nf_out_hook_proc,
	.priv = NULL,
	.pf = PF_INET,
	.hooknum = NF_INET_LOCAL_OUT ,
	.priority = NF_IP_PRI_FIRST ,
};

static struct nf_hook_ops nf_local_in = {
	.hook = nf_in_hook_proc,
	.priv = NULL,
	.pf = PF_INET,
	.hooknum = NF_INET_LOCAL_IN ,
	.priority = NF_IP_PRI_FIRST ,
};

unsigned int nf_out_hook_proc( void * priv, struct sk_buff * skb, 
		        const struct nf_hook_state * state ){
    printk(KERN_INFO"hook_proc");
    return NF_ACCEPT;
}


unsigned int nf_in_hook_proc( void * priv, struct sk_buff * skb, 
		        const struct nf_hook_state * state ){
    printk(KERN_INFO"hook_proc");
    return NF_ACCEPT;
}

static int __init i2pmm_init(void)
{
    printk(KERN_INFO "init i2pmm module !!!\n");
    if( nf_register_net_hook( &init_net, &nf_local_in ) ){
	printk(KERN_INFO"nf_register_hook() failed !\n");
    }else{
	printk(KERN_INFO"nf_register_hook() succeed !\n");
    }
    return 0;
}
 
static void __exit i2pmm_exit(void)
{
    printk(KERN_INFO "exit i2pmm module !!!\n");
    nf_unregister_net_hook(&init_net,  &nf_local_in );
}
 
module_init(i2pmm_init);
module_exit(i2pmm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yyrwkk");
MODULE_DESCRIPTION("Intermediate IP Modification Module");
