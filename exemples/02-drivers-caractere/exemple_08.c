/************************************************************************\
  exemple_08 - Chapitre "Ecriture de driver - peripherique caractere"

  Protection par mutex d'une variable globale

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/sched.h>

#include <asm/uaccess.h>

	static int current_pid = 0;
	DEFINE_MUTEX(mtx_current_pid);


	static ssize_t exemple_read  (struct file * filp, char * buffer,
	                              size_t length, loff_t * offset);


	static struct file_operations fops_exemple = {
		.owner   =  THIS_MODULE,
		.read    =  exemple_read,
	};


	static struct miscdevice exemple_misc_driver = {
		    .minor          = MISC_DYNAMIC_MINOR,
		    .name           = THIS_MODULE->name,
		    .fops           = & fops_exemple,
	};


static int __init exemple_init (void)
{
	return misc_register(& exemple_misc_driver);
}


static void __exit exemple_exit (void)
{
	misc_deregister(& exemple_misc_driver);
}


static ssize_t exemple_read(struct file * filp, char * buffer,
                            size_t length, loff_t * offset)
{
	char k_buffer[2];
	unsigned long delay;

	if (mutex_lock_interruptible(& mtx_current_pid) != 0)
		return -ERESTARTSYS;
	
	current_pid = current->pid;

	delay = jiffies + 10;
	
	/* Boucle de 10 ms pour provoquer artificiellement
	  la collision entre des appels-systeme simultanes.*/
	  
	while (time_before(jiffies, delay))
		schedule();

	if (current_pid == current->pid)
		strcpy(k_buffer, ".");
	else
		strcpy(k_buffer, "#");
	
	mutex_unlock(& mtx_current_pid);
	
	if (length < 2)
		return -ENOMEM;
	if (copy_to_user(buffer, k_buffer, 2) != 0)
		return -EFAULT;
		
	return 1;
}

module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");

