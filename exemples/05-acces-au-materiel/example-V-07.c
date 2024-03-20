// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Driver en mode caracteres"
//
// (c) 2001-2024 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>

#include "gpio-examples.h"



#define EXAMPLE_ARRAY_SIZE 1024
static unsigned long example_array[EXAMPLE_ARRAY_SIZE];
static int           example_array_end;
static spinlock_t    example_array_spl;


static ssize_t example_read(struct file *filp, char *u_buffer, size_t length, loff_t *offset)
{
	unsigned long irqs;
	char k_buffer[80];
	unsigned long value;

	spin_lock_irqsave(&example_array_spl, irqs);

	if (example_array_end == 0) {
		spin_unlock_irqrestore(&example_array_spl, irqs);
		return 0;
	}

	value = example_array[0];

	example_array_end--;
	if (example_array_end > 0)
		memmove(example_array, &(example_array[1]), example_array_end * sizeof(unsigned long));

	spin_unlock_irqrestore(&example_array_spl, irqs);

	snprintf(k_buffer, 80, "%ld\n", value);

	if (length < (strlen(k_buffer) + 1))
		return -ENOMEM;

	if (copy_to_user(u_buffer, k_buffer, strlen(k_buffer) + 1) != 0)
		return -EFAULT;

	return strlen(k_buffer) + 1;
}


static irqreturn_t example_handler(int irq, void *ident)
{
	spin_lock(&example_array_spl);

	if (example_array_end < EXAMPLE_ARRAY_SIZE) {
		example_array[example_array_end] = jiffies;
		example_array_end++;
	}

	spin_unlock(&example_array_spl);

	return IRQ_HANDLED;
}


static const struct file_operations example_fops = {
	.owner =  THIS_MODULE,
	.read  =  example_read,
};


static struct miscdevice example_misc_driver = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = THIS_MODULE->name,
	.fops  = &example_fops,
	.mode  = 0666,
};


static int __init example_init(void)
{
	int err;

	err = gpio_request(EXAMPLE_GPIO_IN, THIS_MODULE->name);
	if (err != 0)
		return err;

	err = gpio_direction_input(EXAMPLE_GPIO_IN);
	if (err != 0) {
		gpio_free(EXAMPLE_GPIO_IN);
		return err;
	}

	spin_lock_init(&example_array_spl);

	err = request_irq(gpio_to_irq(EXAMPLE_GPIO_IN), example_handler,
		IRQF_SHARED | IRQF_TRIGGER_RISING,
		THIS_MODULE->name, THIS_MODULE->name);
	if (err != 0) {
		gpio_free(EXAMPLE_GPIO_IN);
		return err;
	}

	err = misc_register(&example_misc_driver);
	if (err != 0) {
		free_irq(gpio_to_irq(EXAMPLE_GPIO_IN), THIS_MODULE->name);
		gpio_free(EXAMPLE_GPIO_IN);
		return err;
	}

	return 0;
}


static void __exit example_exit(void)
{
	misc_deregister(&example_misc_driver);
	free_irq(gpio_to_irq(EXAMPLE_GPIO_IN), THIS_MODULE->name);
	gpio_free(EXAMPLE_GPIO_IN);
}


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("Spinlock protection of a shared variable");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
