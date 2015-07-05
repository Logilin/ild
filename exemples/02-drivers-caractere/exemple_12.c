/************************************************************************\
  exemple_12 - Chapitre "Ecriture de driver - peripherique caractere"

  Threaded irq pour traiter les interruptions GPIO

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2015 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/module.h>


#include "gpio_exemples.h"


	static irqreturn_t exemple_handler (int irq, void * ident);
	static irqreturn_t exemple_thread  (int irq, void * ident);


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
	
	err = request_threaded_irq(gpio_to_irq(GPIO_IN),
	                           exemple_handler,
	                           exemple_thread,
	                           IRQF_SHARED,
	                           THIS_MODULE->name,
	                           THIS_MODULE->name);
	if (err != 0) {
		gpio_free(GPIO_OUT);
		gpio_free(GPIO_IN);
		return err;
	}
	return 0; 
}


static void __exit exemple_exit (void)
{
	free_irq(gpio_to_irq(GPIO_IN), THIS_MODULE->name);
	gpio_free(GPIO_OUT);
	gpio_free(GPIO_IN);
}


static irqreturn_t exemple_handler(int irq, void * ident)
{
	return IRQ_WAKE_THREAD;
}


static irqreturn_t exemple_thread(int irq, void * ident)
{
	static int value = 1;
	gpio_set_value(GPIO_OUT, value);

	value = 1 - value;
	return IRQ_HANDLED;
}


module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");

