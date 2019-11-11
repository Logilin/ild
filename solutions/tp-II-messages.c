/************************************************************************\
  TP de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Ecriture de driver en mode caractere"

  (c) 2005-2015 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/cdev.h>
	#include <linux/device.h>
	#include <linux/fs.h>
	#include <linux/miscdevice.h>
	#include <linux/module.h>
	#include <linux/sched.h>

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

	#define MSG_NB_MAX 16
	#define MSG_LG_MAX 32
	char table_messages[MSG_NB_MAX][MSG_LG_MAX];
	int table_nb_messages = 0;

static int __init exemple_init (void)
{
	return misc_register(& exemple_misc_driver);
}


static void __exit exemple_exit (void)
{
	misc_deregister(& exemple_misc_driver);
}


static ssize_t exemple_read(struct file * filp, char * u_buffer,
                            size_t length, loff_t * offset)
{
	int lg;

	if (table_nb_messages == 0)
		return 0;
	lg = strlen(table_messages[0]);
	if (length < lg)
		return -EINVAL;

	if (copy_to_user(u_buffer, table_messages[0], lg) != 0)
		return -EFAULT;

	table_nb_messages --;
	if (table_nb_messages > 0) {
		memmove(table_messages[0], table_messages[1], table_nb_messages * MSG_LG_MAX);
	}
	return lg;
}


static ssize_t exemple_write(struct file * filp, const char * u_buffer,
                            size_t length, loff_t * offset)
{
	if (table_nb_messages == MSG_NB_MAX)
		return -ENOMEM;

	if (length >= MSG_LG_MAX)
		return -EINVAL;

	if (copy_from_user(table_messages[table_nb_messages], u_buffer, length) != 0)
		return -EFAULT;

	table_nb_messages ++;

	return length;
}

	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("Exercise solution.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

