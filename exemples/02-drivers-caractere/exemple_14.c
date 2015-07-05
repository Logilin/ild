/************************************************************************\
  exemple_14 - Chapitre "Ecriture de driver - peripherique caractere"

  Declenchement d'une workqueue sur interruption GPIO du Raspberry Pi

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/


#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/workqueue.h>

	// Sortie sur broche 18 (GPIO 24)
	#define RPI_GPIO_OUT 24

	// Entree sur broche 16 (GPIO 23)
	#define RPI_GPIO_IN  23

	static irqreturn_t exemple_handler(int irq, void * ident);

	static void fonction_workqueue(struct work_struct * inutilise);
	static DECLARE_WORK(exemple_workqueue, fonction_workqueue);

static int __init exemple_init (void)
{
	int err;

	if ((err = gpio_request(RPI_GPIO_IN,THIS_MODULE->name)) != 0)
		return err;
		
	if ((err = gpio_request(RPI_GPIO_OUT,THIS_MODULE->name)) != 0) {
		gpio_free(RPI_GPIO_IN);
		return err;
	}
	
	if (((err = gpio_direction_input(RPI_GPIO_IN)) != 0)
	 || ((err = gpio_direction_output(RPI_GPIO_OUT,1)) != 0)) {
		gpio_free(RPI_GPIO_OUT);
		gpio_free(RPI_GPIO_IN);
		return err;
	}

	
	if ((err = request_irq(gpio_to_irq(RPI_GPIO_IN), exemple_handler,
	                       IRQF_SHARED | IRQF_TRIGGER_RISING,
	                       THIS_MODULE->name, THIS_MODULE->name)) != 0) {
		gpio_free(RPI_GPIO_OUT);
		gpio_free(RPI_GPIO_IN);
		return err;
	}
	return 0; 
}


static void __exit exemple_exit (void)
{
	free_irq(gpio_to_irq(RPI_GPIO_IN), THIS_MODULE->name);
	flush_scheduled_work();
	gpio_free(RPI_GPIO_OUT);
	gpio_free(RPI_GPIO_IN);
}


static irqreturn_t exemple_handler(int irq, void * ident)
{
	schedule_work(& exemple_workqueue);
	return IRQ_HANDLED;
}


static void fonction_workqueue(struct work_struct * inutilise)
{
	static int value = 1;

	gpio_set_value(RPI_GPIO_OUT, value);
	value = 1 - value;
}


module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");

