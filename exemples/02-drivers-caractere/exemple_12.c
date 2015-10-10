/************************************************************************\
  exemple_12 - Chapitre "Ecriture de driver - peripherique caractere"

  Utilisation d'une tasklet sur une interruption GPIO

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2015 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/gpio.h>
	#include <linux/interrupt.h>
	#include <linux/module.h>

	#include "gpio_exemples.h"


	static irqreturn_t exemple_handler(int irq, void * ident);

	static void exemple_tasklet_function(unsigned long unused);
	static DECLARE_TASKLET(exemple_tasklet, exemple_tasklet_function, 0);


static int __init exemple_init (void)
{
	int err;

	if ((err = gpio_request(GPIO_IN,THIS_MODULE->name)) != 0)
		return err;

	if ((err = gpio_request(GPIO_OUT,THIS_MODULE->name)) != 0) {
		gpio_free(GPIO_IN);
		return err;
	}

	if (((err = gpio_direction_input(GPIO_IN)) != 0)
	 || ((err = gpio_direction_output(GPIO_OUT,1)) != 0)) {
		gpio_free(GPIO_OUT);
		gpio_free(GPIO_IN);
		return err;
	}

	if ((err = request_irq(gpio_to_irq(GPIO_IN), exemple_handler,
	                       IRQF_SHARED | IRQF_TRIGGER_RISING,
	                       THIS_MODULE->name, THIS_MODULE->name)) != 0) {
		gpio_free(GPIO_OUT);
		gpio_free(GPIO_IN);
		return err;
	}

	return 0;
}



static void __exit exemple_exit (void)
{
	free_irq(gpio_to_irq(GPIO_IN), THIS_MODULE->name);
	tasklet_kill(& exemple_tasklet);
	gpio_free(GPIO_OUT);
	gpio_free(GPIO_IN);
}



static irqreturn_t exemple_handler(int irq, void * ident)
{
	tasklet_schedule(& exemple_tasklet);
	return IRQ_HANDLED;
}



static void exemple_tasklet_function(unsigned long inutilise)
{
	static int value = 1;

	gpio_set_value(GPIO_OUT, value);
	value = 1 - value;
}


	module_init(exemple_init);
	module_exit(exemple_exit);
	MODULE_LICENSE("GPL");
