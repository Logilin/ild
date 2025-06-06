// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Driver reseau"
//
// (c) 2001-2025 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/version.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ip.h>
#include <linux/skbuff.h>


struct net_device *net_dev_ex_0 = NULL;
struct net_device *net_dev_ex_1 = NULL;

struct example_net_dev_priv {

	struct sk_buff *sk_b;

	unsigned char data[ETH_DATA_LEN];
	int data_len;
};


static int example_open(struct net_device *net_dev)
{
	u8 hw_address[6] = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x00 };

	pr_info("%s - %s(%016lx):\n", THIS_MODULE->name, __func__, (unsigned long)net_dev);

	if (net_dev == net_dev_ex_1)
		hw_address[5] = 0x01;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	eth_hw_addr_set(net_dev, hw_address);
#else
	memcpy(net_dev->dev_addr, hw_address, 6);
#endif

	netif_start_queue(net_dev);

	return 0;
}


static int example_stop(struct net_device *net_dev)
{
	pr_info("%s - %s(%016lx):\n", THIS_MODULE->name, __func__, (unsigned long)net_dev);

	netif_tx_disable(net_dev);

	return 0;
}


static irqreturn_t example_irq_rx_handler(int irq, void *irq_id)
{
	unsigned char *data;
	struct sk_buff *sk_b;
	struct net_device *net_dev;
	struct example_net_dev_priv *priv;

	pr_info("%s -%s(%d, %016lx)\n", THIS_MODULE->name, __func__, irq, (unsigned long)irq_id);

	net_dev = (struct net_device *)irq_id;
	priv = netdev_priv(net_dev);
	if (priv == NULL)
		return IRQ_NONE;
	sk_b = dev_alloc_skb(priv->data_len);
	if (!sk_b)
		return IRQ_HANDLED;
	data = skb_put(sk_b, priv->data_len);
	memcpy(data, priv->data, priv->data_len);

	sk_b->dev = net_dev;
	sk_b->protocol = eth_type_trans(sk_b, net_dev);
	sk_b->ip_summed = CHECKSUM_UNNECESSARY;
	netif_rx(sk_b);

	return IRQ_HANDLED;
}


static irqreturn_t example_irq_tx_handler(int irq, void *irq_id)
{
	struct net_device *net_dev;
	struct example_net_dev_priv *priv;

	pr_info("%s -%s(%d, %016lx)\n", THIS_MODULE->name, __func__, irq, (unsigned long)irq_id);

	net_dev = (struct net_device *)irq_id;
	priv = netdev_priv(net_dev);
	if (priv == NULL)
		return IRQ_NONE;

	dev_kfree_skb(priv->sk_b);
	return IRQ_HANDLED;
}


static int example_start_xmit(struct sk_buff *sk_b, struct net_device *src)
{
	struct example_net_dev_priv *dst_priv;
	struct example_net_dev_priv *src_priv;

	struct net_device *dst;
	struct iphdr *ip_header;

	unsigned char *ptr_src;
	unsigned char *ptr_dst;

	char  *data;
	int    len;
	char   short_packet[ETH_ZLEN];

	pr_info("%s -%s(%016lx, %016lx)\n", THIS_MODULE->name, __func__, (unsigned long)sk_b, (unsigned long)src);

	if (src == net_dev_ex_0)
		dst = net_dev_ex_1;
	else
		dst = net_dev_ex_0;

	src_priv = netdev_priv(src);
	dst_priv = netdev_priv(dst);

	data = sk_b->data;
	len  = sk_b->len;

	if (len < ETH_ZLEN) {
		memset(short_packet, 0, ETH_ZLEN);
		memcpy(short_packet, data, len);
		len = ETH_ZLEN;
		data = short_packet;
	}

	if (len > ETH_DATA_LEN)
		return -ENOMEM;

	src_priv->sk_b = sk_b;

	ip_header = (struct iphdr *)(data + sizeof(struct ethhdr));

	ptr_src = (unsigned char *)&(ip_header->saddr);
	ptr_dst = (unsigned char *)&(ip_header->daddr);

	ptr_src += 2;
	*ptr_src = 255 - *ptr_src;
	ptr_dst += 2;
	*ptr_dst = 255 - *ptr_dst;

	ip_header->check = 0;
	ip_header->check = ip_fast_csum((unsigned char *)ip_header, ip_header->ihl);

	memcpy(dst_priv->data, data, len);
	dst_priv->data_len = len;

	example_irq_rx_handler (0, (void *)dst);
	example_irq_tx_handler (0, (void *) src);

	return NETDEV_TX_OK;
}


static int example_hard_header(struct sk_buff *sk_b, struct net_device *net_dev,
	unsigned short type, const void *dst_addr, const void *src_addr, unsigned int len)
{
	struct ethhdr *eth_hdr = NULL;

	eth_hdr = (struct ethhdr *)skb_push(sk_b, ETH_HLEN);
	eth_hdr->h_proto = htons(type);

	if (src_addr == NULL)
		src_addr = net_dev->dev_addr;
	if (dst_addr == NULL)
		dst_addr = net_dev->dev_addr;

	memcpy(eth_hdr->h_source, src_addr, net_dev->addr_len);
	memcpy(eth_hdr->h_dest,   dst_addr, net_dev->addr_len);

	if (eth_hdr->h_dest[ETH_ALEN-1] == 0)
		eth_hdr->h_dest[ETH_ALEN-1] = 1;
	else
		eth_hdr->h_dest[ETH_ALEN-1] = 0;

	return net_dev->hard_header_len;
}


static const struct header_ops example_header_ops = {
	.create = example_hard_header,
};


static const struct net_device_ops example_netdev_ops = {
	.ndo_open       = example_open,
	.ndo_stop       = example_stop,
	.ndo_start_xmit = example_start_xmit,
};


static void example_setup(struct net_device *net_dev)
{
	struct example_net_dev_priv *private_data = NULL;

	pr_info("%s - %s(%016lx)\n", THIS_MODULE->name, __func__, (unsigned long)net_dev);

	ether_setup(net_dev);

	net_dev->netdev_ops = &example_netdev_ops;
	net_dev->header_ops = &example_header_ops;

	net_dev->flags    |= IFF_NOARP;

	private_data = netdev_priv(net_dev);
	memset(private_data, 0, sizeof(struct example_net_dev_priv));
}


static int __init example_init(void)
{

	pr_info("%s - %s()\n", THIS_MODULE->name, __func__);

	net_dev_ex_0 = alloc_netdev(sizeof(struct example_net_dev_priv), "ex%d", NET_NAME_UNKNOWN, example_setup);
	if (net_dev_ex_0 == NULL)
		return -ENOMEM;

	if (register_netdev(net_dev_ex_0) != 0) {
		free_netdev(net_dev_ex_0);
		return -ENODEV;
	}

	net_dev_ex_1 = alloc_netdev(sizeof(struct example_net_dev_priv), "ex%d", NET_NAME_UNKNOWN, example_setup);
	if (net_dev_ex_1 == NULL) {
		unregister_netdev(net_dev_ex_0);
		free_netdev(net_dev_ex_0);
		return -ENOMEM;
	}

	if (register_netdev(net_dev_ex_1) != 0) {
		free_netdev(net_dev_ex_1);
		unregister_netdev(net_dev_ex_0);
		free_netdev(net_dev_ex_0);
		return -ENODEV;
	}

	return 0;
}


static void example_exit(void)
{
	pr_info("%s - %s()\n", THIS_MODULE->name, __func__);

	if (net_dev_ex_1 != NULL) {
		unregister_netdev(net_dev_ex_1);
		free_netdev(net_dev_ex_1);
		net_dev_ex_1 = NULL;
	}

	if (net_dev_ex_0 != NULL) {
		unregister_netdev(net_dev_ex_0);
		free_netdev(net_dev_ex_0);
		net_dev_ex_0 = NULL;
	}
}


module_init(example_init)
module_exit(example_exit)

MODULE_DESCRIPTION("Two virtual linked netdevices.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
