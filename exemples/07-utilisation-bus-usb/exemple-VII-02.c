/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Utilisation du bus USB"

  (c) 2005-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/version.h>
#include <linux/module.h>
#include <linux/usb.h>


#define ID_VENDEUR_EXEMPLE   0x10CF  /* Velleman  */
#define ID_PRODUIT_EXEMPLE   0x5500  /* Kit K8055 */


	static struct usb_device_id   id_table_example [] = {
		{ USB_DEVICE(ID_VENDEUR_EXEMPLE, ID_PRODUIT_EXEMPLE) },
		{ }
	};
	MODULE_DEVICE_TABLE(usb, id_table_example);

	static int  probe_example      (struct usb_interface * intf,
	                                const struct usb_device_id * dev_id);
	static void disconnect_example (struct usb_interface * intf);


	static struct usb_driver usb_driver_example = {
		.name       = "Velleman K8055",
		.id_table   = id_table_example,
		.probe      = probe_example,
		.disconnect = disconnect_example,
	};


static int probe_example(struct usb_interface * intf,
                  const struct usb_device_id  * dev_id)
{
	int i;
	struct usb_host_interface * host_intf;
	struct usb_endpoint_descriptor * endpoint_desc;

	host_intf = intf->cur_altsetting;
	
	for (i = 0; i < host_intf->desc.bNumEndpoints; i++) {
		endpoint_desc = & (host_intf->endpoint[i].desc);
		if (endpoint_desc->bEndpointAddress & USB_DIR_IN) {
			switch (endpoint_desc->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) {
				case USB_ENDPOINT_XFER_BULK:
					printk(KERN_INFO "%s: Bulk in endpoint\n",
					       THIS_MODULE->name);
					break;
				case USB_ENDPOINT_XFER_CONTROL:
					printk(KERN_INFO "%s: Control in endpoint\n",
					       THIS_MODULE->name);
					break;
				case USB_ENDPOINT_XFER_INT:
					printk(KERN_INFO "%s: Interrupt in endpoint\n",
					       THIS_MODULE->name);
					break;
				case USB_ENDPOINT_XFER_ISOC:
					printk(KERN_INFO "%s: Isochronous in endpoint\n",
					       THIS_MODULE->name);
					break;
			}
		} else { // Attention, USB_DIR_OUT vaut 0, un test & ne marche pas !
			switch (endpoint_desc->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) {
				case USB_ENDPOINT_XFER_BULK:
					printk(KERN_INFO "%s: Bulk out endpoint\n",
					       THIS_MODULE->name);
					break;
				case USB_ENDPOINT_XFER_CONTROL:
					printk(KERN_INFO "%s: Control out endpoint\n",
					       THIS_MODULE->name);
					break;
				case USB_ENDPOINT_XFER_INT:
					printk(KERN_INFO "%s: Interrupt out endpoint\n",
					       THIS_MODULE->name);
					break;
				case USB_ENDPOINT_XFER_ISOC:
					printk(KERN_INFO "%s: Isochronous out endpoint\n",
					       THIS_MODULE->name);
					break;
			}
		}
	}
	return 0;
}


static void disconnect_example(struct usb_interface * interface)
{
}


static int __init example_init(void)
{
	int err;

	err = usb_register(& usb_driver_example);
	if (err) {
		printk(KERN_ERR "%s: usb_register(): error %d\n",
		       THIS_MODULE->name, err);
		return err;
	} else {
		printk(KERN_INFO "%s: usb_register(): Ok\n",
		       THIS_MODULE->name);
	}
	return 0;
}


static void __exit example_exit(void)
{
	usb_deregister(& usb_driver_example);
}


	module_init (example_init);
	module_exit (example_exit);

	MODULE_DESCRIPTION("Endpoint analysis.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

