// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Drive ren mode caracteres"
//
// (c) 2001-2021 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/version.h>

#include "gpio-examples.h"


	static irqreturn_t example_top_half(int irq, void *ident);

	static void example_bottom_half(struct work_struct *unused);

	static DECLARE_WORK(example_workqueue, example_bottom_half);


static int __init example_init(void)
{
	int err;

	err = gpio_request(EXAMPLE_GPIO_IN, THIS_MODULE->name);
	if (err != 0)
		return err;

	err = gpio_request(EXAMPLE_GPIO_OUT, THIS_MODULE->name);
	if (err != 0) {
		gpio_free(EXAMPLE_GPIO_IN);
		return err;
	}

	err = gpio_direction_input(EXAMPLE_GPIO_IN);
	if (err != 0) {
		gpio_free(EXAMPLE_GPIO_OUT);
		gpio_free(EXAMPLE_GPIO_IN);
		return err;
	}

	err = gpio_direction_output(EXAMPLE_GPIO_OUT, 0);
	if (err != 0) {
		gpio_free(EXAMPLE_GPIO_OUT);
		gpio_free(EXAMPLE_GPIO_IN);
		return err;
	}

	err = request_irq(gpio_to_irq(EXAMPLE_GPIO_IN), example_top_half,
		IRQF_SHARED | IRQF_TRIGGER_RISING,
		THIS_MODULE->name, THIS_MODULE->name);
	if (err != 0) {
		gpio_free(EXAMPLE_GPIO_OUT);
		gpio_free(EXAMPLE_GPIO_IN);
		return err;
	}

	return 0;

}


static void __exit example_exit(void)
{
	free_irq(gpio_to_irq(EXAMPLE_GPIO_IN), THIS_MODULE->name);
	flush_scheduled_work();
	gpio_free(EXAMPLE_GPIO_OUT);
	gpio_free(EXAMPLE_GPIO_IN);
}


static irqreturn_t example_top_half(int irq, void *ident)
{
	schedule_work(&example_workqueue);
	return IRQ_HANDLED;
}


static void example_bottom_half(struct work_struct *unused)
{
	static int value = 1;

	gpio_set_value(EXAMPLE_GPIO_OUT, value);
	value = 1 - value;
}


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("Workqueue bottom-half implementation");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

