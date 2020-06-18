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


	/* Identification du peripherique gere par notre driver */
	#define ID_VENDEUR_EXAMPLE   0x0000  /* Velleman  */
	#define ID_PRODUIT_EXAMPLE   0x0000  /* Kit K8055 */

	static struct usb_device_id   id_table_example [] = {
		{ USB_DEVICE(ID_VENDEUR_EXAMPLE, ID_PRODUIT_EXAMPLE) },
		{ } /* Par convention on termine par une entree vide */
	};
	MODULE_DEVICE_TABLE(usb, id_table_example);

	/* Fonctions de detection et deconnexion du peripherique */
	static int  probe_example      (struct usb_interface * intf,
	                                const struct usb_device_id * dev_id);
	static void disconnect_example (struct usb_interface * intf);

	/* Representation du pilote de peripherique */
	static struct usb_driver usb_driver_example = {
		.name       = "Velleman K8055",
		.id_table   = id_table_example,
		.probe      = probe_example,
		.disconnect = disconnect_example,
	};


static int probe_example(struct usb_interface * intf,
                  const struct usb_device_id  * dev_id)
{
	printk(KERN_INFO "%s: probe_example()\n",
	       THIS_MODULE->name);
	return 0;
}


static void disconnect_example(struct usb_interface * intf)
{
	printk(KERN_INFO "%s: disconnect_example()\n",
	       THIS_MODULE->name);
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

	MODULE_DESCRIPTION("probe() and disconnect() callbacks invocation.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

