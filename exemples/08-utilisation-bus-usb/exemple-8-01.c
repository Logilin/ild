/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Utilisation du bus USB"

  (c) 2005-2017 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/


	#include <linux/version.h>
	#include <linux/module.h>
	#include <linux/usb.h>


	/* Identification du peripherique gere par notre driver */
	#define ID_VENDEUR_EXEMPLE   0x0000  /* Velleman  */
	#define ID_PRODUIT_EXEMPLE   0x0000  /* Kit K8055 */

	static struct usb_device_id   id_table_exemple [] = {
		{ USB_DEVICE(ID_VENDEUR_EXEMPLE, ID_PRODUIT_EXEMPLE) },
		{ } /* Par convention on termine par une entree vide */
	};
	MODULE_DEVICE_TABLE(usb, id_table_exemple);

	/* Fonctions de detection et deconnexion du peripherique */
	static int  probe_exemple      (struct usb_interface * intf,
	                                const struct usb_device_id * dev_id);
	static void disconnect_exemple (struct usb_interface * intf);

	/* Representation du pilote de peripherique */
	static struct usb_driver usb_driver_exemple = {
		.name       = "Velleman K8055",
		.id_table   = id_table_exemple,
		.probe      = probe_exemple,
		.disconnect = disconnect_exemple,
	};


static int probe_exemple(struct usb_interface * intf,
                  const struct usb_device_id  * dev_id)
{
	printk(KERN_INFO "%s: probe_exemple()\n",
	       THIS_MODULE->name);
	return 0;
}


static void disconnect_exemple(struct usb_interface * intf)
{
	printk(KERN_INFO "%s: disconnect_exemple()\n",
	       THIS_MODULE->name);
}


static int __init exemple_init(void)

{
	int err;

	err = usb_register(& usb_driver_exemple);
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


static void __exit exemple_exit(void)
{
	usb_deregister(& usb_driver_exemple);
}


	module_init (exemple_init);
	module_exit (exemple_exit);

	MODULE_DESCRIPTION("probe() and disconnect() callbacks invocation.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

