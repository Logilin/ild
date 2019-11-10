/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Acces au materiel"

  (c) 2005-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/cdev.h>
	#include <linux/device.h>
	#include <linux/fs.h>
	#include <linux/gpio.h>
	#include <linux/miscdevice.h>
	#include <linux/module.h>
	#include <linux/uaccess.h>
	#include <asm/uaccess.h>

	#include "gpio-exemples.h"


	static ssize_t example_read  (struct file * filp, char * buffer,
	                              size_t length, loff_t * offset);

	static ssize_t example_write (struct file * filp, const char * buffer,
	                              size_t length, loff_t * offset);

	static struct file_operations fops_exemple = {
		.owner   =  THIS_MODULE,
		.read    =  example_read,
		.write   =  example_write,
	};

	static struct miscdevice example_misc_driver = {
		    .minor          = MISC_DYNAMIC_MINOR,
		    .name           = THIS_MODULE->name,
		    .fops           = & fops_exemple,
	};


static int __init example_init (void)
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

	if ((err = misc_register(& example_misc_driver)) != 0) {
		gpio_free(EXEMPLE_GPIO_OUT);
		gpio_free(EXEMPLE_GPIO_IN);
		return err;
	}

	return 0;
}


static void __exit example_exit (void)
{
	misc_deregister(& example_misc_driver);
	gpio_free(EXEMPLE_GPIO_OUT);
	gpio_free(EXEMPLE_GPIO_IN);
}


static ssize_t example_read(struct file * filp, char * buffer,
                            size_t length, loff_t * offset)
{
	char k_buffer [8];

	if (length < 2)
		return 0;
	sprintf(k_buffer, "%d\n", gpio_get_value(EXEMPLE_GPIO_IN));
	if (copy_to_user(buffer, k_buffer, 2) != 0)
		return -EFAULT;

	return 2;
}


static ssize_t example_write(struct file * filp, const char * buffer,
                             size_t length, loff_t * offset)
{
	char k_buffer[80];
	int val;

	if (length > 79)
		return -EINVAL;
	if (copy_from_user(k_buffer, buffer, length) != 0)
		return -EFAULT;
	if (sscanf(k_buffer, "%d", & val) != 1)
		return -EINVAL;

	gpio_set_value(EXEMPLE_GPIO_OUT, val & 0x01);

	return length;
}


	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("Read and write system call on GPIO pins.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

