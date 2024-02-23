// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
//
// (c) 2001-2024 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>


#define MSG_NB_MAX 16
#define MSG_LG_MAX 32

char   messages_array[MSG_NB_MAX][MSG_LG_MAX];
int    messages_count = 0;
DEFINE_MUTEX(messages_mutex);
DECLARE_WAIT_QUEUE_HEAD(messages_read_wq);
DECLARE_WAIT_QUEUE_HEAD(messages_write_wq);


static ssize_t messages_read(struct file *filp, char *u_buffer, size_t length, loff_t *offset)
{
	ssize_t ret = 0;

	if (mutex_lock_interruptible(&messages_mutex) != 0)
		return -ERESTARTSYS;

	while (messages_count == 0) {
		mutex_unlock(&messages_mutex);
		if (wait_event_interruptible(messages_read_wq, messages_count != 0) != 0)
                        return -ERESTARTSYS;
		if (mutex_lock_interruptible(&messages_mutex) != 0)
	                return -ERESTARTSYS;
	}

	ret = strlen(messages_array[0]) - (*offset);
	if (ret <= 0) {
		messages_count--;
		if (messages_count > 0)
			memmove(messages_array[0], messages_array[1], messages_count * MSG_LG_MAX);
		wake_up_interruptible(&messages_write_wq);
		mutex_unlock(&messages_mutex);
		return 0;
	}

	if (ret > length)
		ret = length;

	if (copy_to_user(u_buffer, messages_array[*offset], ret) != 0)
		ret = -EFAULT;
	else
		(*offset) += ret;

	mutex_unlock(&messages_mutex);

	return ret;
}


static ssize_t messages_write(struct file *filp, const char *u_buffer, size_t length, loff_t *offset)
{
	int ret;

	if (mutex_lock_interruptible(&messages_mutex) != 0)
		return -ERESTARTSYS;

	while (messages_count >= MSG_NB_MAX) {
		mutex_unlock(&messages_mutex);
		if (wait_event_interruptible(messages_write_wq, messages_count < MS_NB_MAX) != 0)
                        return -ERESTARTSYS;
		if (mutex_lock_interruptible(&messages_mutex) != 0)
	                return -ERESTARTSYS;
	}

	ret = -EINVAL;
	if (length >= MSG_LG_MAX)
		goto unlock_out;

	ret = -EFAULT;
	if (copy_from_user(messages_array[messages_count], u_buffer, length) != 0)
		goto unlock_out;

	messages_count++;

	wake_up_interruptible(&messages_read_wq);

	ret = length;

	unlock_out:

	mutex_unlock(&messages_mutex);

	return ret;
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

