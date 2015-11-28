/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Ecriture de driver USB"

  (c) 2005-2015 Christophe Blaess
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
	static ssize_t exemple_read    (struct file *, char __user *,
	                                size_t, loff_t *);

	static void exemple_write_callback (struct urb *);
	static void exemple_read_callback  (struct urb *);


	static struct file_operations exemple_file_operations = {
		.owner   = THIS_MODULE,
		.open    = exemple_open,
		.release = exemple_release,
		.write   = exemple_write,
		.read    = exemple_read,
	};


	static struct usb_class_driver exemple_usb_class_driver = {
		.name = "usb/velleman%d",
		.fops = & exemple_file_operations,
		.minor_base = 0,
	};


	static int exemple_open_count = 0;
	static DEFINE_MUTEX(exemple_mtx);

	static struct usb_device * exemple_usb_device = NULL;

	static struct usb_endpoint_descriptor * exemple_out_endpoint  = NULL;
	static struct urb                     * exemple_out_urb       = NULL;
	static char                           * exemple_out_buffer    = NULL;
	static int                              exemple_out_busy      = 0;
	static DEFINE_MUTEX(exemple_out_mtx);
	static DECLARE_WAIT_QUEUE_HEAD(exemple_out_wq);
	#define EXEMPLE_OUT_BUFFER_SIZE  8

	static struct usb_endpoint_descriptor * exemple_in_endpoint  = NULL;
	static struct urb                     * exemple_in_urb       = NULL;
	static char                           * exemple_in_buffer    = NULL;
	static int                              exemple_in_busy      = 0;
	static DEFINE_MUTEX(exemple_in_mtx);
	static DECLARE_WAIT_QUEUE_HEAD(exemple_in_wq);
	#define EXEMPLE_IN_BUFFER_SIZE  8


static int exemple_probe(struct usb_interface * intf,
                         const struct usb_device_id * dev_id)
{
	int i;
	int err;
	struct usb_host_interface * host_intf;
	struct usb_endpoint_descriptor * endpoint_desc;

	exemple_usb_device = usb_get_dev(interface_to_usbdev(intf));

	host_intf = intf->cur_altsetting;
	
	for (i = 0; i < host_intf->desc.bNumEndpoints; i++) {
		endpoint_desc = & (host_intf->endpoint[i].desc);

		if (endpoint_desc->bEndpointAddress & USB_DIR_IN) {
			if ((endpoint_desc->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_INT)
				exemple_in_endpoint = endpoint_desc;
		} else {
			if ((endpoint_desc->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_INT)
				exemple_out_endpoint = endpoint_desc;
		}
	}

	err = -ENODEV;
	if (exemple_out_endpoint == NULL) {
		printk(KERN_INFO "%s: no output interrupt endpoint in probe()\n",
		       THIS_MODULE->name);
		goto exit_null;
	}

	if (exemple_in_endpoint == NULL) {
		printk(KERN_INFO "%s: no input interrupt endpoint in probe()\n",
		       THIS_MODULE->name);
		goto exit_null;
	}

	err = -EINVAL;
	if (exemple_out_endpoint->wMaxPacketSize < EXEMPLE_OUT_BUFFER_SIZE) {
		printk(KERN_INFO "%s: output interrupt max packet size too small\n",
		       THIS_MODULE->name);
		goto exit_null;
	}

	if (exemple_in_endpoint->wMaxPacketSize < EXEMPLE_IN_BUFFER_SIZE) {
		printk(KERN_INFO "%s: input interrupt max packet size too small\n",
		       THIS_MODULE->name);
		goto exit_null;
	}

	err = -ENOMEM;
	exemple_out_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (exemple_out_urb == NULL)
		goto exit_null;

	exemple_in_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (exemple_in_urb == NULL)
		goto exit_free_out_urb;

	exemple_out_buffer = kmalloc(EXEMPLE_OUT_BUFFER_SIZE, GFP_KERNEL);
	if (exemple_out_buffer == NULL)
		goto exit_free_in_urb;
	
	exemple_in_buffer = kmalloc(EXEMPLE_IN_BUFFER_SIZE, GFP_KERNEL);
	if (exemple_in_buffer == NULL)
		goto exit_free_out_buf;

	err = usb_register_dev(intf, & exemple_usb_class_driver);
	if (err != 0)
		goto exit_free_in_buf;

	exemple_in_busy  = 0;
	exemple_out_busy = 0;

	printk(KERN_INFO "%s: minor=%d\n", THIS_MODULE->name, intf->minor);

	return 0;

	exit_free_in_buf:
		kfree(exemple_in_buffer);
		exemple_in_buffer  = NULL;
	exit_free_out_buf:
		kfree(exemple_out_buffer);
		exemple_out_buffer = NULL;
	exit_free_in_urb:
		usb_free_urb(exemple_in_urb);
		exemple_in_urb  = NULL;
	exit_free_out_urb:
		usb_free_urb(exemple_out_urb);
		exemple_out_urb = NULL;
	exit_null:
		exemple_in_endpoint  = NULL;
		exemple_out_endpoint = NULL;
		exemple_usb_device   = NULL;
		return err;
}


static void exemple_disconnect(struct usb_interface * intf)
{
	usb_deregister_dev(intf, & exemple_usb_class_driver);
	exemple_usb_device   = NULL;

	mutex_lock(& exemple_in_mtx);
	kfree(exemple_in_buffer);
	exemple_in_buffer    = NULL;
	usb_free_urb(exemple_in_urb);
	exemple_in_urb       = NULL;
	exemple_in_endpoint  = NULL;
	mutex_unlock(& exemple_in_mtx);

	mutex_lock(& exemple_out_mtx);
	kfree(exemple_out_buffer);
	usb_free_urb(exemple_out_urb);
	exemple_out_urb      = NULL;
	exemple_out_buffer   = NULL;
	exemple_out_endpoint = NULL;
	mutex_unlock(& exemple_out_mtx);
}


static int exemple_open(struct inode *inode, struct file *filp)
{
	if (mutex_lock_interruptible(& exemple_mtx))
		return -ERESTARTSYS;
	if (exemple_open_count > 0) {
		mutex_unlock(& exemple_mtx);
		return -EBUSY;
	}
	if (exemple_usb_device == NULL) {
		mutex_unlock(& exemple_mtx);
		return -ENODEV;
	}

	exemple_open_count ++;
	mutex_unlock(& exemple_mtx);
	return 0;
}


static int exemple_release(struct inode *inode, struct file *filp)
{
	if (mutex_lock_interruptible(& exemple_mtx))
		return -ERESTARTSYS;

	exemple_open_count --;
	mutex_unlock(& exemple_mtx);

	return 0;
}


static ssize_t exemple_write (struct file * file,
                              const char __user * data, size_t length, loff_t * offset)
{
	int o0, o1, o2, o3, o4, o5, o6, o7;
	char * buffer;
	int err;
	
	buffer = kmalloc(length, GFP_KERNEL);
	if (buffer == NULL) {
		return -ENOMEM;
	}
	if (copy_from_user(buffer, data, length)) {
		kfree(buffer);
		return -EFAULT;
	}
	err = (sscanf(buffer, "%d %d %d %d %d %d %d %d", &o0, &o1, &o2, &o3, &o4, &o5, &o6, &o7) != 8);
	kfree(buffer);
	if (err)
		return -EINVAL;


	if (mutex_lock_interruptible(& exemple_out_mtx) != 0)
		return -ERESTARTSYS;

	if (exemple_usb_device == NULL) {
		mutex_unlock(& exemple_out_mtx);
		return -ENODEV;
	}

	// Attendre qu'il n'y ait plus d'ecriture en cours (normalement inutile).
	while(exemple_out_busy) {
		err = wait_event_interruptible_timeout(exemple_out_wq, ! exemple_out_busy, HZ);
		if (err < 0) {
			mutex_unlock(& exemple_out_mtx);
			return err;
		}
	}

	// Preparer la requete.
	exemple_out_busy = 1;
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
	                 NULL,
	                 exemple_out_endpoint->bInterval);

	// Lancer la requete.
	err = usb_submit_urb(exemple_out_urb, GFP_KERNEL);
	if (err != 0) {
		exemple_out_busy = 0;
		mutex_unlock(& exemple_out_mtx);
		return err;
	}

	// Attendre la fin de l'ecriture.
	while(exemple_out_busy) {
		err = wait_event_interruptible_timeout(exemple_out_wq, ! exemple_out_busy, HZ);
		if (err < 0) {
			exemple_out_busy = 0;
			mutex_unlock(& exemple_out_mtx);
			return err;
		}
	}

	// En cas d'echec indiquer une erreur.
	if (exemple_out_urb->status != 0) {
			exemple_out_busy = 0;
			mutex_unlock(& exemple_out_mtx);
			return -EIO;
	}

	mutex_unlock(& exemple_out_mtx);
	return length;
}


static void exemple_write_callback(struct urb * urb)
{
	exemple_out_busy = 0;
	wake_up_interruptible(& exemple_out_wq);
}


static ssize_t exemple_read(struct file * file, char __user * data,
                            size_t length, loff_t * offset)
{
	int err;
	size_t lg;
	static char k_buffer[64];

	// Ne faire une nouvelle requete que si (*offset) est a zero.
	if ((*offset) == 0) {

		if (mutex_lock_interruptible(& exemple_in_mtx) != 0)
			return -ERESTARTSYS;

		if (exemple_usb_device == NULL) {
			mutex_unlock(& exemple_in_mtx);
			return -ENODEV;
		}

		// Attendre qu'il n'y ait plus de lecture en cours (normalement inutile).
		while (exemple_in_busy) {
			err = wait_event_interruptible_timeout(exemple_in_wq, ! exemple_in_busy, HZ);
			if (err < 0) {
				mutex_unlock(& exemple_in_mtx);
				return err;
			}
		}

		// Indiquer une lecture en cours
		exemple_in_busy = 1;

		// Lancer la requete
		usb_fill_int_urb(exemple_in_urb,
			             exemple_usb_device,
			             usb_rcvintpipe(exemple_usb_device, exemple_in_endpoint->bEndpointAddress),
			             exemple_in_buffer,
						 exemple_in_endpoint->wMaxPacketSize,
						 exemple_read_callback,
						 NULL,
						 exemple_in_endpoint->bInterval);

		err = usb_submit_urb(exemple_in_urb, GFP_KERNEL);
		if (err != 0) {
			exemple_in_busy = 0;
			mutex_unlock(& exemple_in_mtx);
			return err;
		}

		// Attendre la fin de la lecture
		while (exemple_in_busy) {
			err = wait_event_interruptible_timeout(exemple_in_wq, ! exemple_in_busy, HZ);
			if (err < 0) {
				exemple_in_busy = 0;
				mutex_unlock(& exemple_in_mtx);
				return err;
			}
		}
		// En cas d'echec retourner EOF.
		if (exemple_in_urb->status != 0) {
				exemple_in_busy = 0;
				mutex_unlock(& exemple_in_mtx);
				return 0;
		}
		
		// Renvoyer le resultat
		snprintf(k_buffer, 64, "%d %d %d %d %d %d %d %d\n", 
			            0xFF & exemple_in_buffer[0],
			            0xFF & exemple_in_buffer[1],
			            0xFF & exemple_in_buffer[2],
			            0xFF & exemple_in_buffer[3],
			            0xFF & exemple_in_buffer[4],
			            0xFF & exemple_in_buffer[5],
			            0xFF & exemple_in_buffer[6],
			            0xFF & exemple_in_buffer[7]);

		mutex_unlock(& exemple_in_mtx);

	}
	
	lg = strlen(k_buffer) + 1 - (*offset);
	if (lg <= 0)
			return 0;		

	if (lg > length)
		lg = length;
	
	if (copy_to_user(data, &(k_buffer[*offset]), lg) != 0)
		return -EFAULT;
	*offset += lg;
	
	return lg;
}


static void exemple_read_callback(struct urb * urb)
{
	exemple_in_busy = 0;
	wake_up_interruptible(& exemple_in_wq);
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

	MODULE_DESCRIPTION("read() system call implementation.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

