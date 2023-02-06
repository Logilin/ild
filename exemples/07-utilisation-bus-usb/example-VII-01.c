// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Utilisation du bus USB"
//
// (c) 2001-2023 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/version.h>
#include <linux/module.h>
#include <linux/usb.h>


static int example_probe(struct usb_interface *intf, const struct usb_device_id  *dev_id)
{
	pr_info("%s: probe_example()\n", THIS_MODULE->name);
	return 0;
}


static void example_disconnect(struct usb_interface *intf)
{
	pr_info("%s: disconnect_example()\n", THIS_MODULE->name);
}


#define EXAMPLE_VENDOR_ID   0x0000  /* Velleman  */
#define EXAMPLE_PRODUCT_ID  0x0000  /* Kit K8055 */

static struct usb_device_id example_id_table[] = {
	{ USB_DEVICE(EXAMPLE_VENDOR_ID, EXAMPLE_PRODUCT_ID) },
	{ }
};
MODULE_DEVICE_TABLE(usb, example_id_table);


static struct usb_driver example_usb_driver = {
	.name       = "Velleman K8055",
	.id_table   = example_id_table,
	.probe      = example_probe,
	.disconnect = example_disconnect,
};


static int __init example_init(void)

{
	int err;

	err = usb_register(&example_usb_driver);
	if (err) {
		pr_err("%s: usb_register(): error %d\n", THIS_MODULE->name, err);
		return err;
	}
	pr_info("%s: usb_register(): Ok\n", THIS_MODULE->name);

	return 0;
}


static void __exit example_exit(void)
{
	usb_deregister(&example_usb_driver);
}


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("probe() and disconnect() callbacks invocation.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

