/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Utilisation du bus USB"

  (c) 2005-2017 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/version.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <asm/uaccess.h>


	#define EXEMPLE_ID_VENDEUR   0x10CF  /* Velleman  */
	#define EXEMPLE_ID_PRODUIT   0x5500  /* Kit K8055 */


	static struct usb_device_id   exemple_id_table [] = {
		{ USB_DEVICE(EXEMPLE_ID_VENDEUR, EXEMPLE_ID_PRODUIT) },
		{ }
	};
	MODULE_DEVICE_TABLE(usb, exemple_id_table);


	static int  exemple_probe      (struct usb_interface * intf,
	                                const struct usb_device_id * dev_id);
	static void exemple_disconnect (struct usb_interface * intf);


	static struct usb_driver exemple_usb_driver = {
		.name       = "Velleman K8055",
		.id_table   = exemple_id_table,
		.probe      = exemple_probe,
		.disconnect = exemple_disconnect,
	};

	static int     exemple_open    (struct inode *, struct file *);
	static int     exemple_release (struct inode *, struct file *);
	static ssize_t exemple_write   (struct file *, const char __user *,
	                                size_t, loff_t *);
	static void    exemple_write_callback (struct urb *);

	static struct file_operations exemple_file_operations = {
		.owner   = THIS_MODULE,
		.open    = exemple_open,
		.release = exemple_release,
		.write   = exemple_write,
	};

	static struct usb_class_driver exemple_usb_class_driver = {
		.name       = "usb/velleman%d",
		.fops       = & exemple_file_operations,
		.minor_base = 0,
	};


	static struct usb_device              * exemple_usb_device   = NULL;
	static struct usb_endpoint_descriptor * exemple_out_endpoint = NULL;
	static struct urb                     * exemple_out_urb      = NULL;
	static char                           * exemple_out_buffer   = NULL;
	#define EXEMPLE_OUT_BUFFER_SIZE  8


static int exemple_probe(struct usb_interface * intf,
                         const struct usb_device_id * dev_id)
{
	int i;
	int err;
	struct usb_host_interface * host_intf;
	struct usb_endpoint_descriptor * endpoint_desc;

	/* Stocker l'identifiant du peripherique. */	
	exemple_usb_device = usb_get_dev(interface_to_usbdev(intf));

	host_intf = intf->cur_altsetting;
	
	/* Rechercher un endpoint INTERRUPT OUT */
	for (i = 0; i < host_intf->desc.bNumEndpoints; i++) {
		endpoint_desc = & (host_intf->endpoint[i].desc);
		if (! (endpoint_desc->bEndpointAddress & USB_DIR_IN)) {
			switch (endpoint_desc->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) {
				case USB_ENDPOINT_XFER_INT:
					exemple_out_endpoint = endpoint_desc;
					break;
				default:
					break;
			}
		}
	}

	if (exemple_out_endpoint == NULL) {
		printk(KERN_INFO "%s: no endpoint interrupt out in probe()\n",
		       THIS_MODULE->name);
		return -ENODEV;
	}

	/* Verifier si le endpoint INTERRUPT OUT a une bande passante suffisante. */
	if (exemple_out_endpoint->wMaxPacketSize < EXEMPLE_OUT_BUFFER_SIZE) {
		printk(KERN_INFO "%s: Interrupt out max packet size too small\n",
		       THIS_MODULE->name);
		exemple_out_endpoint = NULL;
		exemple_usb_device  = NULL;
		return -EINVAL;
	}

	/* Allouer un URB pour les requetes en ecriture a venir */
	exemple_out_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (exemple_out_urb == NULL) {
		exemple_out_endpoint = NULL;
		exemple_usb_device = NULL;
		return -ENOMEM;
	}

	exemple_out_buffer = kmalloc(EXEMPLE_OUT_BUFFER_SIZE, GFP_KERNEL);
	if (exemple_out_buffer == NULL) {
		usb_free_urb(exemple_out_urb);
		exemple_out_urb = NULL;
		exemple_out_endpoint = NULL;
		exemple_usb_device   = NULL;
		return -ENOMEM;
	}
	
	err = usb_register_dev(intf, & exemple_usb_class_driver);
	if (err != 0) {
		kfree(exemple_out_buffer);
		exemple_out_buffer = NULL;
		usb_free_urb(exemple_out_urb);
		exemple_out_urb = NULL;
		exemple_out_endpoint = NULL;
		exemple_usb_device   = NULL;
		return err;
	}
	return 0;
}


static void exemple_disconnect(struct usb_interface * intf)
{
	usb_deregister_dev(intf, & exemple_usb_class_driver);
	kfree(exemple_out_buffer);
	usb_free_urb(exemple_out_urb);
	exemple_out_urb = NULL;
	exemple_out_buffer   = NULL;
	exemple_out_endpoint = NULL;
	exemple_usb_device   = NULL;
}


static int exemple_open(struct inode *inode, struct file *filp)
{
	return 0;
}


static int exemple_release(struct inode *inode, struct file *filp)
{
	return 0;
}


static ssize_t exemple_write (struct file * file,
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
	
	exemple_out_buffer[0] = o0 & 0xFF; 
	exemple_out_buffer[1] = o1 & 0xFF;
	exemple_out_buffer[2] = o2 & 0xFF;
	exemple_out_buffer[3] = o3 & 0xFF;
	exemple_out_buffer[4] = o4 & 0xFF;
	exemple_out_buffer[5] = o5 & 0xFF;
	exemple_out_buffer[6] = o6 & 0xFF;
	exemple_out_buffer[7] = o7 & 0xFF;

	usb_fill_int_urb(exemple_out_urb,
	                 exemple_usb_device,
	                 usb_sndintpipe(exemple_usb_device,
					        exemple_out_endpoint->bEndpointAddress),
	                 exemple_out_buffer,
					 EXEMPLE_OUT_BUFFER_SIZE,
					 exemple_write_callback,
					 NULL, /* donnees privees non utilisees */
					 exemple_out_endpoint->bInterval);

	err = usb_submit_urb(exemple_out_urb, GFP_KERNEL);
	if (err == 0)
		return length;
	return err;
}


static void exemple_write_callback(struct urb * urb)
{
}


static int __init exemple_init(void)
{
	int err;

	err = usb_register(& exemple_usb_driver);
	if (err) {
		printk(KERN_ERR "%s: Erreur dans usb_register(): %d\n",
		       THIS_MODULE->name, err);
		return err;
	}
	return 0;
}


static void __exit exemple_exit(void)
{
	usb_deregister(& exemple_usb_driver);
}


	module_init (exemple_init);
	module_exit (exemple_exit);

	MODULE_DESCRIPTION("write() system call implementation.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

