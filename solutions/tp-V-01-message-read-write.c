// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
//
// (c) 2001-202 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>


#define MSG_NB_MAX 16
#define MSG_LG_MAX 32
char messages_array[MSG_NB_MAX][MSG_LG_MAX];
int messages_count = 0;


static ssize_t messages_read(struct file *filp, char *u_buffer, size_t length, loff_t *offset)
{
	ssize_t lg;

	if (messages_count == 0)
		return 0;

	lg = strlen(messages_array[0]) - (*offset);
	if (lg <= 0) {
		messages_count--;
		if (messages_count > 0)
			memmove(messages_array[0], messages_array[1], messages_count * MSG_LG_MAX);
		return 0;
	}
	if (lg > length)
		lg = length;

	if (copy_to_user(u_buffer, messages_array[*offset], lg) != 0)
		return -EFAULT;

	(*offset) += lg;

	return lg;
}


static ssize_t messages_write(struct file *filp, const char *u_buffer, size_t length, loff_t *offset)
{
	if (messages_count == MSG_NB_MAX)
		return -ENOMEM;

	if (length >= MSG_LG_MAX)
		return -EINVAL;

	if (copy_from_user(messages_array[messages_count], u_buffer, length) != 0)
		return -EFAULT;

	messages_count++;

	return length;
}


static const struct file_operations messages_fops = {
	.owner   =  THIS_MODULE,
	.read    =  messages_read,
	.write   =  messages_write,
};


static struct miscdevice messages_misc_driver = {
	    .minor = MISC_DYNAMIC_MINOR,
	    .name  = "message-queue",
	    .fops  = &messages_fops,
	    .mode  = 0666,
};


static int __init messages_init(void)
{
	return misc_register(&messages_misc_driver);
}


static void __exit messages_exit(void)
{
	misc_deregister(&messages_misc_driver);
}


module_init(messages_init);
module_exit(messages_exit);

MODULE_DESCRIPTION("Exercise solution.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

