// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
//
// (c) 2001-2024 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/version.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>

#define NB_EVENT_MAX  64

struct s_timed_event {
	int      data;
	time64_t time;
};

struct s_timed_event Events[NB_EVENT_MAX];
int Nb_events = 0;



static int event_table_empty(void)
{
	return (Nb_events <= 0);
}



static int event_table_full(void)
{
	return (Nb_events >= NB_EVENT_MAX);
}



static int add_event_to_table(int data)
{
	if (event_table_full())
		return -ENOSPC;
	Events[Nb_events].data = data;
	Events[Nb_events].time = ktime_get_real_ns();
	Nb_events ++;
	return 0;
}



static void remove_first_event(void)
{
	Nb_events --;
	if (! event_table_empty())
		memmove(Events, &(Events[1]), Nb_events * sizeof(struct s_timed_event));
}



static void format_first_event(char *string, size_t max)
{
	snprintf(string, max - 1, "[%lld] %d\n", Events[0].time, Events[0].data);
}



static ssize_t log_read(struct file *filp, char __user *u_buffer, size_t max, loff_t *offset)
{
	char string[64];
	int  chars_to_return;

	if (event_table_empty())
		return 0;

	format_first_event(string, 64);
	chars_to_return = strlen(string) - (*offset);

	if (chars_to_return <= 0) {
		*offset = 0;

		remove_first_event();
		if (event_table_empty())
			return 0;
		format_first_event(string, 64);
		chars_to_return = strlen(string) - (*offset);
		if (chars_to_return <= 0)
			return 0;
	}

	if (chars_to_return > max)
		chars_to_return = max;
	if (copy_to_user(u_buffer, &(string[*offset]), chars_to_return) != 0)
		return  -EFAULT;
	(*offset) += chars_to_return;

	return chars_to_return;
}



static ssize_t log_write(struct file * filp, const char __user * u_buffer, size_t nb, loff_t * unused)
{
	int value;
	int err;

	if ((err = kstrtoint_from_user(u_buffer, nb, 10, &value)) != 0)
		return err;

	if (add_event_to_table(value) != 0)
		return -ENOSPC;

	return nb;
}



#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
	static const struct proc_ops log_fops = {
		.proc_read   = log_read,
		.proc_write  = log_write,
	};
#else
	static const struct file_operations log_fops = {
		.owner	= THIS_MODULE,
		.read   = log_read,
		.write  = log_write,
	};
#endif



static int __init log_init(void)
{
	if (proc_create("log", 0666, NULL, &log_fops) == NULL)
		return -EBUSY;

	return 0;
}


static void __exit log_exit(void)
{
	remove_proc_entry("log", NULL);
}


module_init(log_init);
module_exit(log_exit);

MODULE_DESCRIPTION("Exercise III-02 solution.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
