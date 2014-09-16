/************************************************************************\
  exemple_13 - Chapitre "Ecriture de driver - peripherique caractere"

  Utilisation d'une tasklet

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <asm/io.h>


	static irqreturn_t exemple_handler(int irq, void * ident);

	static void fonction_tasklet (unsigned long inutile);
	static DECLARE_TASKLET(exemple_tasklet, fonction_tasklet, 0);

static int __init exemple_init (void)
{
	int err;

	if (request_region(0x378, 3, THIS_MODULE->name) == NULL)
		return -EBUSY;

	err = request_irq(7, exemple_handler, IRQF_SHARED, THIS_MODULE->name, THIS_MODULE->name);
	if (err != 0) {
		release_region(0x378, 3);
		return err;
	}
	outb(0x10, 0x37A);
	return 0; 
}


static void __exit exemple_exit (void)
{
	outb(0, 0x37A);
	free_irq(7, THIS_MODULE->name);
	tasklet_kill(& exemple_tasklet);
	release_region(0x378, 3);
}


static irqreturn_t exemple_handler(int irq, void * ident)
{
	if ((inb(0x379) & 0x40) == 0)
		return IRQ_NONE;

	tasklet_schedule(& exemple_tasklet);
	return IRQ_HANDLED;
}


static void fonction_tasklet(unsigned long inutile)
{
	static int valeur = 0;

	outb(valeur, 0x378);
	valeur = 0xFF - valeur;
}

module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");

