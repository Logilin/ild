/************************************************************************\
  exemple_13 - Chapitre "Ecriture de driver - peripherique caractere"

  Utilisation d'une tasklet sur une interruption clavier

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/fs.h>


	static int irq_number = 1;
	module_param(irq_number, int, 0444);


	static irqreturn_t exemple_handler(int irq, void * ident);

	static void fonction_tasklet(unsigned long unused);
	static DECLARE_TASKLET(exemple_tasklet, fonction_tasklet, 0);


static int __init exemple_init (void)
{
	int err;
	
	err = request_irq(irq_number, exemple_handler, IRQF_SHARED, THIS_MODULE->name, THIS_MODULE->name);
	return err;
}


static void __exit exemple_exit (void)
{
	free_irq(irq_number, THIS_MODULE->name);
	tasklet_kill(& exemple_tasklet);
}


static irqreturn_t exemple_handler(int irq, void * ident)
{
	printk(KERN_INFO "%s - tasklet_schedule() at %lu\n",
	       THIS_MODULE->name, jiffies);
	tasklet_schedule(& exemple_tasklet);
	return IRQ_HANDLED;
}


static void fonction_tasklet(unsigned long inutilise)
{
	printk(KERN_INFO "%s - fonction_tasklet() at %lu\n",
		THIS_MODULE->name, jiffies);
}


module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");

