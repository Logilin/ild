/************************************************************************\
  exemple_09_A - Chapitre "Ecriture de driver - peripherique caractere"

  Driver permettant les entrees-sorties sur broches GPIO d'un Raspberry Pi

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <asm/uaccess.h>

	// Sortie sur broche 18 (GPIO 24)
	#define RPI_GPIO_OUT 24

	// Entree sur broche 16 (GPIO 23)
	#define RPI_GPIO_IN  23


	static ssize_t exemple_read  (struct file * filp, char * buffer,
	                              size_t length, loff_t * offset);
	static ssize_t exemple_write (struct file * filp, const char * buffer,
	                              size_t length, loff_t * offset);


	static struct file_operations fops_exemple = {
		.owner   =  THIS_MODULE,
		.read    =  exemple_read,
		.write   =  exemple_write,
	};


	static struct miscdevice exemple_misc_driver = {
		    .minor          = MISC_DYNAMIC_MINOR,
		    .name           = THIS_MODULE->name,
		    .fops           = & fops_exemple,
	};


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
	if ((err = misc_register(& exemple_misc_driver)) != 0) {
		gpio_free(RPI_GPIO_OUT);
		gpio_free(RPI_GPIO_IN);
		return err;
	}		
	return 0;
}


static void __exit exemple_exit (void)
{
	misc_deregister(& exemple_misc_driver);
	gpio_free(RPI_GPIO_OUT);
	gpio_free(RPI_GPIO_IN);
}


static ssize_t exemple_read(struct file * filp, char * buffer,
                            size_t length, loff_t * offset)
{
	char k_buffer [8];
	if (length < 2)
		return 0;
	sprintf(k_buffer, "%d\n", gpio_get_value(RPI_GPIO_IN) & 0x01);
	if (copy_to_user(buffer, k_buffer, 2) != 0)
		return -EFAULT;
	return 2;
}


static ssize_t exemple_write(struct file * filp, const char * buffer,
                             size_t length, loff_t * offset)
{
	char k_buffer[80];
	int val;

	if (length > 79)
		return -EINVAL;
	if (copy_from_user(k_buffer, buffer, length) != 0)
		return -EFAULT;
	if (sscanf(k_buffer, "%x", & val) != 1)
		return -EINVAL;
	gpio_set_value(RPI_GPIO_OUT, val);
	return length;
}


module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");

