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

#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>

#include "gpio-examples.h"


	static irqreturn_t example_handler(int irq, void *ident);

	static ssize_t example_read(struct file *filp, char *buffer, size_t length, loff_t *offset);


	static const struct file_operations example_fops = {
		.owner =  THIS_MODULE,
		.read  =  example_read,
	};

	static struct miscdevice example_misc_driver = {
		    .minor = MISC_DYNAMIC_MINOR,
		    .name  = THIS_MODULE->name,
		    .fops  = &example_fops,
	};


	#define EXAMPLE_BUFFER_SIZE 1024
	static unsigned long example_buffer[EXAMPLE_BUFFER_SIZE];
	static int           example_buffer_end;
	static spinlock_t    example_buffer_spl;


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

	spin_lock_init(&example_buffer_spl);

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


static ssize_t example_read(struct file *filp, char *u_buffer, size_t length, loff_t *offset)
{
	unsigned long irqs;
	char k_buffer[80];

	spin_lock_irqsave(&example_buffer_spl, irqs);

	if (example_buffer_end == 0) {
		spin_unlock_irqrestore(&example_buffer_spl, irqs);
		return 0;
	}

	snprintf(k_buffer, 80, "%ld\n", example_buffer[0]);

	example_buffer_end--;
	if (example_buffer_end > 0)
		memmove(example_buffer, &(example_buffer[1]), example_buffer_end * sizeof(unsigned long));

	spin_unlock_irqrestore(&example_buffer_spl, irqs);

	if (length < (strlen(k_buffer) + 1))
		return -ENOMEM;

	if (copy_to_user(u_buffer, k_buffer, strlen(k_buffer) + 1) != 0)
		return -EFAULT;

	return strlen(k_buffer) + 1;
}


static irqreturn_t example_handler(int irq, void *ident)
{
	spin_lock(&example_buffer_spl);

	if (example_buffer_end < EXAMPLE_BUFFER_SIZE) {
		example_buffer[example_buffer_end] = jiffies;
		example_buffer_end++;
	}

	spin_unlock(&example_buffer_spl);

	return IRQ_HANDLED;
}


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("Spinlock protection of a shared variable");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

