/************************************************************************\
  exemple_09_B - Chapitre "Ecriture de driver - peripherique caractere"

  Driver permettant les entrees-sorties sur port parallele

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <asm/uaccess.h>


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

	if (request_region(0x378, 3, THIS_MODULE->name) == NULL)
		return -EBUSY;
	
	if ((err = misc_register(& exemple_misc_driver)) != 0) {
		release_region(0x378, 3);
		return err;
	}		
	return 0;
}


static void __exit exemple_exit (void)
{
	misc_deregister(& exemple_misc_driver);
	release_region(0x378, 3);
}


static ssize_t exemple_read(struct file * filp, char * buffer,
                        size_t length, loff_t * offset)
{
	char k_buffer [4];
	if (length < 3)
		return 0;
	sprintf(k_buffer, "%02X ", inb(0x379));
	if (copy_to_user(buffer, k_buffer, 3) != 0)
		return -EFAULT;
	return 3;
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
	outb(val, 0x378);
	return length;
}

module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");

