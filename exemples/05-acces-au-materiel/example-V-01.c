// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Driver en mode caracteres"
//
// (c) 2001-2022 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>

#include "gpio-examples.h"


static ssize_t example_read(struct file *filp, char *u_buffer, size_t length, loff_t *offset)
{
	char k_buffer[8];

	if (length < 2)
		return 0;
	sprintf(k_buffer, "%d\n", gpio_get_value(EXAMPLE_GPIO_IN));
	if (copy_to_user(u_buffer, k_buffer, 2) != 0)
		return -EFAULT;

	return 2;
}


static ssize_t example_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
	char k_buffer[80];
	int val;

	if (length > 79)
		return -ENOMEM;

	if (copy_from_user(k_buffer, buffer, length) != 0)
		return -EFAULT;
	k_buffer[length] = '\0';

	if (kstrtoint(k_buffer, 10, &val) != 0)
		return -EINVAL;

	gpio_set_value(EXAMPLE_GPIO_OUT, val & 0x01);

	return length;
}


static const struct file_operations example_fops = {
	.owner   =  THIS_MODULE,
	.read    =  example_read,
	.write   =  example_write,
};


static struct miscdevice example_misc_driver = {
	.minor          = MISC_DYNAMIC_MINOR,
	.name           = THIS_MODULE->name,
	.fops           = &example_fops,
	.mode           = 0666,
};


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

	err = misc_register(&example_misc_driver);
	if (err != 0) {
		gpio_free(EXAMPLE_GPIO_OUT);
		gpio_free(EXAMPLE_GPIO_IN);
		return err;
	}

	return 0;
}


static void __exit example_exit(void)
{
	misc_deregister(&example_misc_driver);
	gpio_free(EXAMPLE_GPIO_OUT);
	gpio_free(EXAMPLE_GPIO_IN);
}


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("Read and write system call on GPIO pins.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
