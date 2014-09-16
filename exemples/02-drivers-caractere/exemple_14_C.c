/************************************************************************\
  exemple_14 - Chapitre "Ecriture de driver - peripherique caractere"

  Declenchement d'une workqueue sur interruption clavier

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/module.h>


	static int irq_number = 1;
	module_param(irq_number, int, 0444);

	static irqreturn_t exemple_handler(int irq, void * ident);

	static void fonction_workqueue(struct work_struct * inutilise);
	static DECLARE_WORK(exemple_workqueue, fonction_workqueue);


static int __init exemple_init (void)
{
	int err;
	
	err = request_irq(irq_number, exemple_handler, IRQF_SHARED, THIS_MODULE->name, THIS_MODULE->name);
	return err;
}


static void __exit exemple_exit (void)
{
	free_irq(irq_number, THIS_MODULE->name);
	flush_scheduled_work();
}


static irqreturn_t exemple_handler(int irq, void * ident)
{
	printk(KERN_INFO "%s - schedule_work() at %lu\n",
	                  THIS_MODULE->name, jiffies);
	schedule_work(& exemple_workqueue);
	return IRQ_HANDLED;
}


static void fonction_workqueue(struct work_struct * inutilise)
{
	printk(KERN_INFO "%s - fonction_workqueue() at %ld\n",
		THIS_MODULE->name, jiffies);
}


module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");

