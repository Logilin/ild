/************************************************************************\
  exemple_11 - Chapitre "Ecriture de driver - peripherique caractere"

  Interception des interruptions du clavier

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/interrupt.h>
#include <linux/module.h>


	static int irq_number = 1;
	module_param(irq_number, int, 0444);

	static irqreturn_t exemple_handler(int irq, void * ident);


static int __init exemple_init (void)
{
	int err;
	
	err = request_irq(irq_number, exemple_handler, IRQF_SHARED, THIS_MODULE->name, THIS_MODULE->name);
	if (err != 0) {
		return err;
	}
	return 0; 
}


static void __exit exemple_exit (void)
{
	free_irq(irq_number, THIS_MODULE->name);
}


static irqreturn_t exemple_handler(int irq, void * ident)
{
	printk(KERN_INFO "%s - exemple_handler() executed at %lu\n",
	       THIS_MODULE->name, jiffies);
	return IRQ_HANDLED;
}

module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");

