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

static int dev_in_proc( struct sk_buff * skb, struct net_device * dev,
		        struct packet_type * ptype,struct net_device *oridev);

/**********function define end************/

static struct nf_hook_ops nf_local_out = {
	.hook = nf_out_hook_proc,
	.priv = NULL,
	.pf = PF_INET,
	.hooknum = NF_INET_POST_ROUTING,
	.priority = NF_IP_PRI_FIRST ,
};

static struct packet_type dev_in = {
	.type = htons(0xf4f0),
	.func = dev_in_proc
};

int net_server_ip = 167880896;  // 192.168.1.10
int net_client_ip = 184658112;  // 192.168.1.11

// int net_server_ip = 184658112;  // 192.168.1.11
// int net_client_ip = 167880896;  // 192.168.1.10
// #define ETH_NAME "ens33"
// unsigned char src_mac[ETH_ALEN]={0x00,0x0c,0x29,0xb7,0x94,0xb6};/*src mac address*/ 
unsigned char dst_mac[ETH_ALEN]={0xff,0xff,0xff,0xff,0xff,0xff};   /*dst mac address*/

unsigned int nf_out_hook_proc( void * priv, struct sk_buff * skb, 
		        const struct nf_hook_state * state ){
	struct iphdr *ipheader;   // IP header
	struct ethhdr *ethheader = NULL;
	struct sk_buff * new_skb;
	int ret = -1;
	// int index = 0;
	unsigned char * dataheader = NULL;
	ipheader = ip_hdr(skb); // retrieve the IP headers from the packet
    // if( ipheader->protocol == IPPROTO_UDP) { 
	// 	// printk(KERN_ERR "send to server");
	// 	if(ipheader->daddr != net_server_ip &&  ipheader->daddr != net_client_ip) return NF_ACCEPT;
	if( skb->len + 6 > 1500) {
		printk(KERN_INFO"eth frame is too long \n");
		goto out;
	}
		
	if( skb_headroom(skb) < ETH_HLEN + 6 ){
		new_skb = skb_copy_expand(skb,ETH_HLEN+6,0,GFP_ATOMIC);
		kfree_skb(skb);
		if( !new_skb){
			goto out;
		}
	}else{
		new_skb = skb_copy(skb,GFP_ATOMIC);
		kfree_skb(skb);
	}
	
	dataheader = skb_push(new_skb,6);
	*(dataheader + 0) = 'y';
	*(dataheader + 1) = 'y';
	*(dataheader + 2) = 'r';
	*(dataheader + 3) = 'w';
	*(dataheader + 4) = 'k';
	*(dataheader + 5) = 'k';

	ethheader = (struct ethhdr*)skb_push(new_skb, ETH_HLEN);
	memcpy(ethheader->h_source, new_skb->dev->dev_addr, ETH_ALEN);
	memcpy(ethheader->h_dest, dst_mac, ETH_ALEN);
	ethheader->h_proto = htons(0xf4f0);

	if(0 > dev_queue_xmit(new_skb)){
		printk(KERN_ERR "send pkt error");
		goto out;
	}
	printk(KERN_INFO ">>>>>>>>>>>>>>>>>\n");
	printk(KERN_INFO "send success\n");
	printk(KERN_INFO "<<<<<<<<<<<<<<<<<\n");
	ret = 0;
        
out:
	if(ret != 0 && NULL != new_skb){
		kfree_skb(new_skb);
	}

	return NF_STOLEN;
}

static int dev_in_proc( struct sk_buff * skb, struct net_device * dev,
		        struct packet_type * ptype,struct net_device *oridev){
	int idx;
	skb = skb_share_check(skb,GFP_ATOMIC);
	if(!skb) goto out;
	printk("heere<<<<<<<<<<<<<<<");
	
	for(idx=0;idx<6;idx++){
		printk(KERN_INFO "%c\r\n",*((char*)(skb)+idx));
	}

	skb_pull_rcsum(skb,6);
	skb->protocol = htons(ETH_P_IP);
	netif_rx(skb);
    return NET_RX_SUCCESS;
out:
	return NET_RX_DROP;
}

static int __init i2pmm_init(void)
{
    printk(KERN_INFO "init i2pmm module !!!\n");
    

	if( nf_register_net_hook( &init_net, &nf_local_out ) ){
		printk(KERN_INFO"nf_out register_hook() failed !\n");
    }else{
		printk(KERN_INFO"nf_out register_hook() succeed !\n");
    }
	dev_add_pack( &dev_in );
    return 0;
}
 
static void __exit i2pmm_exit(void)
{
    printk(KERN_INFO "exit i2pmm module !!!\n");
    
	nf_unregister_net_hook(&init_net,  &nf_local_out );
	dev_remove_pack( &dev_in );
}
 
module_init(i2pmm_init);
module_exit(i2pmm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yyrwkk");
MODULE_DESCRIPTION("Intermediate IP Modification Module");
MODULE_VERSION("v1.0.0");
