/************************************************************************\
  exemple_11 - Chapitre "Ecriture de driver - peripherique caractere"

  Gestion des interruptions du port parallele

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <asm/io.h>


	static irqreturn_t exemple_handler(int irq, void * ident);


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
	release_region(0x378, 3);
}

static irqreturn_t exemple_handler(int irq, void * ident)
{
	static int val = 0;

	/* Verifions si la broche 10 est bien levee, ce qui indique
	   une interruption probable. */
	if ((inb(0x379) & 0x40) == 0)
		return IRQ_NONE;

	outb(val, 0x378);
	val = 0xFF - val;

	return IRQ_HANDLED;
}

module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");

