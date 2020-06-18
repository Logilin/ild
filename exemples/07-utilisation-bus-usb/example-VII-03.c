/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Utilisation du bus USB"

  (c) 2005-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/version.h>
	#include <linux/module.h>
	#include <linux/slab.h>
	#include <linux/usb.h>
	#include <asm/uaccess.h>
	#include <linux/uaccess.h>


	#define EXAMPLE_ID_VENDEUR   0x10CF  /* Velleman  */
	#define EXAMPLE_ID_PRODUIT   0x5500  /* Kit K8055 */


	static struct usb_device_id   example_id_table [] = {
		{ USB_DEVICE(EXAMPLE_ID_VENDEUR, EXAMPLE_ID_PRODUIT) },
		{ }
	};
	MODULE_DEVICE_TABLE(usb, example_id_table);


	static int  example_probe      (struct usb_interface * intf,
	                                const struct usb_device_id * dev_id);
	static void example_disconnect (struct usb_interface * intf);


	static struct usb_driver example_usb_driver = {
		.name       = "Velleman K8055",
		.id_table   = example_id_table,
		.probe      = example_probe,
		.disconnect = example_disconnect,
	};

	static int     example_open    (struct inode *, struct file *);
	static int     example_release (struct inode *, struct file *);
	static ssize_t example_write   (struct file *, const char __user *,
	                                size_t, loff_t *);
	static void    example_write_callback (struct urb *);

	static struct file_operations example_file_operations = {
		.owner   = THIS_MODULE,
		.open    = example_open,
		.release = example_release,
		.write   = example_write,
	};

	static struct usb_class_driver example_usb_class_driver = {
		.name       = "usb/velleman%d",
		.fops       = & example_file_operations,
		.minor_base = 0,
	};


	static struct usb_device              * example_usb_device   = NULL;
	static struct usb_endpoint_descriptor * example_out_endpoint = NULL;
	static struct urb                     * example_out_urb      = NULL;
	static char                           * example_out_buffer   = NULL;
	#define EXAMPLE_OUT_BUFFER_SIZE  8


static int example_probe(struct usb_interface * intf,
                         const struct usb_device_id * dev_id)
{
	int i;
	int err;
	struct usb_host_interface * host_intf;
	struct usb_endpoint_descriptor * endpoint_desc;

	/* Stocker l'identifiant du peripherique. */	
	example_usb_device = usb_get_dev(interface_to_usbdev(intf));

	host_intf = intf->cur_altsetting;
	
	/* Rechercher un endpoint INTERRUPT OUT */
	for (i = 0; i < host_intf->desc.bNumEndpoints; i++) {
		endpoint_desc = & (host_intf->endpoint[i].desc);
		if (! (endpoint_desc->bEndpointAddress & USB_DIR_IN)) {
			switch (endpoint_desc->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) {
				case USB_ENDPOINT_XFER_INT:
					example_out_endpoint = endpoint_desc;
					break;
				default:
					break;
			}
		}
	}

	if (example_out_endpoint == NULL) {
		printk(KERN_INFO "%s: no endpoint interrupt out in probe()\n",
		       THIS_MODULE->name);
		return -ENODEV;
	}

	/* Verifier si le endpoint INTERRUPT OUT a une bande passante suffisante. */
	if (example_out_endpoint->wMaxPacketSize < EXAMPLE_OUT_BUFFER_SIZE) {
		printk(KERN_INFO "%s: Interrupt out max packet size too small\n",
		       THIS_MODULE->name);
		example_out_endpoint = NULL;
		example_usb_device  = NULL;
		return -EINVAL;
	}

	/* Allouer un URB pour les requetes en ecriture a venir */
	example_out_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (example_out_urb == NULL) {
		example_out_endpoint = NULL;
		example_usb_device = NULL;
		return -ENOMEM;
	}

	example_out_buffer = kmalloc(EXAMPLE_OUT_BUFFER_SIZE, GFP_KERNEL);
	if (example_out_buffer == NULL) {
		usb_free_urb(example_out_urb);
		example_out_urb = NULL;
		example_out_endpoint = NULL;
		example_usb_device   = NULL;
		return -ENOMEM;
	}
	
	err = usb_register_dev(intf, & example_usb_class_driver);
	if (err != 0) {
		kfree(example_out_buffer);
		example_out_buffer = NULL;
		usb_free_urb(example_out_urb);
		example_out_urb = NULL;
		example_out_endpoint = NULL;
		example_usb_device   = NULL;
		return err;
	}
	return 0;
}


static void example_disconnect(struct usb_interface * intf)
{
	usb_deregister_dev(intf, & example_usb_class_driver);
	kfree(example_out_buffer);
	usb_free_urb(example_out_urb);
	example_out_urb = NULL;
	example_out_buffer   = NULL;
	example_out_endpoint = NULL;
	example_usb_device   = NULL;
}


static int example_open(struct inode *inode, struct file *filp)
{
	return 0;
}


static int example_release(struct inode *inode, struct file *filp)
{
	return 0;
}


static ssize_t example_write (struct file * file,
                              const char __user * data,
                              size_t length, loff_t * offset)
{
	int o0, o1, o2, o3, o4, o5, o6, o7;
	char * buffer;
	int err;
	
	buffer = kmalloc(length, GFP_KERNEL);
	if (buffer == NULL)
		return -ENOMEM;

	if (copy_from_user(buffer, data, length)) {
		kfree(buffer);
		return -EINVAL;
	}
	/* On attend 8 chiffres decimaux à envoyer à la carte. */
	err = (sscanf(buffer, "%d %d %d %d %d %d %d %d", &o0, &o1, &o2, &o3, &o4, &o5, &o6, &o7) != 8);
	/* Le buffer peut etre libere immediatement. */
	kfree(buffer);
	if (err)
		return -EINVAL;
	
	example_out_buffer[0] = o0 & 0xFF; 
	example_out_buffer[1] = o1 & 0xFF;
	example_out_buffer[2] = o2 & 0xFF;
	example_out_buffer[3] = o3 & 0xFF;
	example_out_buffer[4] = o4 & 0xFF;
	example_out_buffer[5] = o5 & 0xFF;
	example_out_buffer[6] = o6 & 0xFF;
	example_out_buffer[7] = o7 & 0xFF;

	usb_fill_int_urb(example_out_urb,
	                 example_usb_device,
	                 usb_sndintpipe(example_usb_device,
					        example_out_endpoint->bEndpointAddress),
	                 example_out_buffer,
					 EXAMPLE_OUT_BUFFER_SIZE,
					 example_write_callback,
					 NULL, /* donnees privees non utilisees */
					 example_out_endpoint->bInterval);

	err = usb_submit_urb(example_out_urb, GFP_KERNEL);
	if (err == 0)
		return length;
	return err;
}


static void example_write_callback(struct urb * urb)
{
}


static int __init example_init(void)
{
	int err;

	err = usb_register(& example_usb_driver);
	if (err) {
		printk(KERN_ERR "%s: Erreur dans usb_register(): %d\n",
		       THIS_MODULE->name, err);
		return err;
	}
	return 0;
}


static void __exit example_exit(void)
{
	usb_deregister(& example_usb_driver);
}


	module_init (example_init);
	module_exit (example_exit);

	MODULE_DESCRIPTION("write() system call implementation.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

