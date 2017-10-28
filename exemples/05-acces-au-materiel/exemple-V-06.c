/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Acces au materiel"

  (c) 2005-2017 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/


	#include <linux/gpio.h>
	#include <linux/interrupt.h>
	#include <linux/module.h>

	#include "gpio-exemples.h"


	static irqreturn_t exemple_handler (int irq, void * ident);
	static irqreturn_t exemple_thread  (int irq, void * ident);


static int __init exemple_init (void)
{
	int err;

	if ((err = gpio_request(EXEMPLE_GPIO_IN,THIS_MODULE->name)) != 0)
		return err;

	if ((err = gpio_request(EXEMPLE_GPIO_OUT,THIS_MODULE->name)) != 0) {
		gpio_free(EXEMPLE_GPIO_IN);
		return err;
	}

	if (((err = gpio_direction_input(EXEMPLE_GPIO_IN)) != 0)
	 || ((err = gpio_direction_output(EXEMPLE_GPIO_OUT, 0)) != 0)) {
		gpio_free(EXEMPLE_GPIO_OUT);
		gpio_free(EXEMPLE_GPIO_IN);
		return err;
	}

	err = request_threaded_irq(gpio_to_irq(EXEMPLE_GPIO_IN),
	                           exemple_handler,
	                           exemple_thread,
	                           IRQF_SHARED,
	                           THIS_MODULE->name,
	                           THIS_MODULE->name);
	if (err != 0) {
		gpio_free(EXEMPLE_GPIO_OUT);
		gpio_free(EXEMPLE_GPIO_IN);
		return err;
	}
	return 0;
}


static void __exit exemple_exit (void)
{
	free_irq(gpio_to_irq(EXEMPLE_GPIO_IN), THIS_MODULE->name);
	gpio_free(EXEMPLE_GPIO_OUT);
	gpio_free(EXEMPLE_GPIO_IN);
}


static irqreturn_t exemple_handler(int irq, void * ident)
{
	return IRQ_WAKE_THREAD;
}


static irqreturn_t exemple_thread(int irq, void * ident)
{
	static int value = 1;
	gpio_set_value(EXEMPLE_GPIO_OUT, value);

	value = 1 - value;
	return IRQ_HANDLED;
}


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("Threaded interrupt handler.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

