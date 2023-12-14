#include <linux/init.h> /* Needed for the macros */
#include <linux/kernel.h> 
#include <linux/module.h> /* Needed by all modules */
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/udp.h>

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

int net_server_ip = 167880896;  // 192.168.1.10
int net_client_ip = 184658112;  // 192.168.1.11


unsigned char *skb_unpush(struct sk_buff *skb, unsigned int len)
{
	skb->data += len;
	skb->len  -= len;
	// if (unlikely(skb->data<skb->head))
	// 	skb_under_panic(skb, len, __builtin_return_address(0));
	return skb->data;
}

unsigned int nf_out_hook_proc( void * priv, struct sk_buff * skb, 
		        const struct nf_hook_state * state ){
	struct iphdr *ipheader;   // IP header
	// int index = 0;
	unsigned char * data_hdr = NULL;
    //struct udphdr *udpheader; // UDP header
	if(!skb) return NF_ACCEPT;
	ipheader = ip_hdr(skb); // retrieve the IP headers from the packet
    if( ipheader->protocol == IPPROTO_UDP) { 
        //udph = udp_hdr(skb);
        //if(ntohs(udph->dest) == 53) {
        //    return NF_ACCEPT; // accept UDP packet
        //}
		if(ipheader->daddr != net_server_ip ) return NF_ACCEPT;
		// for( index =0;index <14;index++){
		// 	printk(KERN_INFO"%x",*(skb->data + index));
		// }

		data_hdr = skb_push(skb,6);

		*(data_hdr+0) = 'y';
		*(data_hdr+1) = 'y';
		*(data_hdr+2) = 'r';
		*(data_hdr+3) = 'w';
		*(data_hdr+4) = 'k';
		*(data_hdr+5) = 'k';

		printk(KERN_INFO"send to server");
    }

    return NF_ACCEPT;
}

unsigned int nf_in_hook_proc( void * priv, struct sk_buff * skb, 
		        const struct nf_hook_state * state ){
	struct iphdr *ipheader;   // IP header
	char recv_i2pmm[7] = {0};
    //struct udphdr *udpheader; // UDP header
	if(!skb) return NF_ACCEPT;
	ipheader = ip_hdr(skb); // retrieve the IP headers from the packet
    if( ipheader->protocol == IPPROTO_UDP) { 
        //udph = udp_hdr(skb);
        //if(ntohs(udph->dest) == 53) {
        //    return NF_ACCEPT; // accept UDP packet
        //}
		if(ipheader->saddr != net_client_ip ) return NF_ACCEPT;
		// for( index =0;index <14;index++){
		// 	printk(KERN_INFO"%x",*(skb->data + index));
		// }

		*(recv_i2pmm+0) = *(skb->data + 0);
		*(recv_i2pmm+1) = *(skb->data + 1);
		*(recv_i2pmm+2) = *(skb->data + 2);
		*(recv_i2pmm+3) = *(skb->data + 3);
		*(recv_i2pmm+4) = *(skb->data + 4);
		*(recv_i2pmm+5) = *(skb->data + 5);
		*(recv_i2pmm+6) = '\0';

		skb_unpush(skb,6);
		printk(KERN_INFO"%s",recv_i2pmm);
		printk(KERN_INFO"recv from client");
    }

    return NF_ACCEPT;
}

static int __init i2pmm_init(void)
{
    printk(KERN_INFO "init i2pmm module !!!\n");
    if( nf_register_net_hook( &init_net, &nf_local_in ) ){
		printk(KERN_INFO"nf_in register_hook() failed !\n");
    }else{
		printk(KERN_INFO"nf_in register_hook() succeed !\n");
    }

	if( nf_register_net_hook( &init_net, &nf_local_out ) ){
		printk(KERN_INFO"nf_out register_hook() failed !\n");
    }else{
		printk(KERN_INFO"nf_out register_hook() succeed !\n");
    }
    return 0;
}
 
static void __exit i2pmm_exit(void)
{
    printk(KERN_INFO "exit i2pmm module !!!\n");
    nf_unregister_net_hook(&init_net,  &nf_local_in );
	nf_unregister_net_hook(&init_net,  &nf_local_out );
}
 
module_init(i2pmm_init);
module_exit(i2pmm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yyrwkk");
MODULE_DESCRIPTION("Intermediate IP Modification Module");
