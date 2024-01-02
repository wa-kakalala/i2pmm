#include <linux/init.h> /* Needed for the macros */
#include <linux/kernel.h> 
#include <linux/module.h> /* Needed by all modules */
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/timer.h>
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

typedef struct stat_info_s{
	unsigned short send_cnt;
	unsigned short recv_cnt;
}stat_info_s;

typedef struct vni_type_s {
	unsigned char stuid[4] ;
	unsigned short    seq  ;
}vni_type_s;

// timer
struct timer_list timer;

volatile stat_info_s    stat_info    = {0,0};
volatile stat_info_s    stat_info_rz = {0,0};  // return zero

int net_server_ip = 167880896;  // 192.168.1.10
int net_client_ip = 184658112;  // 192.168.1.11

// int net_server_ip = 184658112;  // 192.168.1.11
// int net_client_ip = 167880896;  // 192.168.1.10
// #define ETH_NAME "ens33"
// unsigned char src_mac[ETH_ALEN]={0x00,0x0c,0x29,0xb7,0x94,0xb6};/*src mac address*/ 
unsigned char dst_mac[ETH_ALEN]={0xff,0xff,0xff,0xff,0xff,0xff};   /*dst mac address*/

unsigned int nf_out_hook_proc( void * priv, struct sk_buff * skb, 
		        const struct nf_hook_state * state ){
	//struct iphdr *ipheader;   // IP header
	struct ethhdr *ethheader = NULL;
	struct sk_buff * new_skb;
	int ret = -1;
	vni_type_s * pvni_type_s;
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
	
	pvni_type_s = (vni_type_s *)skb_push(new_skb,sizeof(vni_type_s));
	pvni_type_s->stuid[0] = 1; pvni_type_s->stuid[1] = 7;
	pvni_type_s->stuid[2] = 1; pvni_type_s->stuid[3] = 8;
	pvni_type_s->seq = htons((stat_info.send_cnt));

	
	ethheader = (struct ethhdr*)skb_push(new_skb, ETH_HLEN);
	memcpy(ethheader->h_source, new_skb->dev->dev_addr, ETH_ALEN);
	memcpy(ethheader->h_dest, dst_mac, ETH_ALEN);
	ethheader->h_proto = htons(0xf4f0);

	if(0 > dev_queue_xmit(new_skb)){
		printk(KERN_ERR "send pkt error");
		goto out;
	}
	ret = 0;
	stat_info.send_cnt ++;
	stat_info_rz.send_cnt ++;
        
out:
	if(ret != 0 && NULL != new_skb){
		kfree_skb(new_skb);
	}

	return NF_STOLEN;
}

static int dev_in_proc( struct sk_buff * skb, struct net_device * dev,
		        struct packet_type * ptype,struct net_device *oridev){
	vni_type_s * pvni_type_s;
	skb = skb_share_check(skb,GFP_ATOMIC);
	if(!skb) goto out;

	pvni_type_s = (vni_type_s *)(skb->data);

	printk(KERN_INFO" -------------------------\r\n");
	printk(KERN_INFO"|stuid:\t%d%d%d%d\t\t|\r\n",pvni_type_s->stuid[0],
	                                           pvni_type_s->stuid[1],
											   pvni_type_s->stuid[2],
											   pvni_type_s->stuid[3]);
	printk(KERN_INFO"|-------------------------|\r\n");										   
	printk(KERN_INFO"|seq:\t%d\t\t|\r\n",ntohs(pvni_type_s->seq));
	printk(KERN_INFO" -------------------------\r\n");

	skb_pull_rcsum(skb,sizeof(vni_type_s));
	skb->protocol = htons(ETH_P_IP);
	netif_rx(skb);
	stat_info.recv_cnt   ++;
	stat_info_rz.recv_cnt++;
    return NET_RX_SUCCESS;
out:
	return NET_RX_DROP;
}

/*timer work function*/
static void timer_work(struct timer_list* param_timer){
	printk(KERN_INFO"\r\nstat info:");
	printk(KERN_INFO" -------------------------");
	printk(KERN_INFO"|send total: %d\t\t|",stat_info.send_cnt);
	printk(KERN_INFO"|send info : %u.%upps\t|",stat_info_rz.send_cnt / 60, stat_info_rz.send_cnt % 60 *10/60); stat_info_rz.send_cnt=0;
	printk(KERN_INFO"|-------------------------");
	printk(KERN_INFO"|recv total: %d\t\t|",stat_info.recv_cnt);
	printk(KERN_INFO"|recv info : %u.%upps\t|",stat_info_rz.recv_cnt / 60,stat_info_rz.recv_cnt%60 *10  / 60); stat_info_rz.recv_cnt=0;
	printk(KERN_INFO" -------------------------\r\n");

	mod_timer(&timer, jiffies + 60 *HZ);
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

	timer.expires = jiffies + 60 *HZ;  //interval is 60s
	timer_setup(&timer,timer_work,0);
	add_timer(&timer);/*start timer*/

    return 0;
}
 
static void __exit i2pmm_exit(void)
{
    printk(KERN_INFO "exit i2pmm module !!!\n");
    
	nf_unregister_net_hook(&init_net,  &nf_local_out );
	dev_remove_pack( &dev_in );
	del_timer(&timer); //close timer
}
 
module_init(i2pmm_init);
module_exit(i2pmm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yyrwkk");
MODULE_DESCRIPTION("Intermediate IP Modification Module");
MODULE_VERSION("v1.0.0");
