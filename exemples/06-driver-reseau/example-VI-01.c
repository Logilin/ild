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
#include <linux/version.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>


struct net_device *net_dev_ex;


static int example_open(struct net_device *net_dev)
{
	u8 hw_address[6] = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x00 };

	pr_info("%s - %s(%016lx):\n", THIS_MODULE->name, __func__, (unsigned long)net_dev);

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


static int example_start_xmit(struct sk_buff *sk_b, struct net_device *src)
{
	pr_info("%s -%s(%016lx, %016lx)\n", THIS_MODULE->name, __func__, (unsigned long)sk_b, (unsigned long)src);

	dev_kfree_skb(sk_b);

	return NETDEV_TX_OK;
}


static const struct net_device_ops example_netdev_ops = {
	.ndo_open       = example_open,
	.ndo_stop       = example_stop,
	.ndo_start_xmit = example_start_xmit,
};


static void example_setup(struct net_device *net_dev)
{
	pr_info("%s - %s(%016lx)\n", THIS_MODULE->name, __func__, (unsigned long)net_dev);

	ether_setup(net_dev);

	net_dev->netdev_ops = &example_netdev_ops;
}


static int __init example_init(void)
{

	pr_info("%s - %s()\n", THIS_MODULE->name, __func__);

	net_dev_ex = alloc_netdev(0, "ex%d", NET_NAME_UNKNOWN, example_setup);
	if (net_dev_ex == NULL)
		return -ENOMEM;

	if (register_netdev(net_dev_ex) != 0) {
		unregister_netdev(net_dev_ex);
		free_netdev(net_dev_ex);
		return -ENODEV;
	}
	return 0;
}


static void example_exit(void)
{
	pr_info("%s - %s()\n", THIS_MODULE->name, __func__);

	if (net_dev_ex != NULL) {
		unregister_netdev(net_dev_ex);
		free_netdev(net_dev_ex);
	}
}


module_init(example_init)
module_exit(example_exit)

MODULE_DESCRIPTION("False device implementation.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
