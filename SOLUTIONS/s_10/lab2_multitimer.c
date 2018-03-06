/* **************** LFD331:4.2 s_10/lab2_multitimer.c **************** */
/*
 * The code herein is: Copyright the Linux Foundation, 2015
 *
 * This Copyright is retained for the purpose of protecting free
 * redistribution of source.
 *
 *     URL:    http://training.linuxfoundation.org
 *     email:  trainingquestions@linuxfoundation.org
 *
 * This code is distributed under Version 2 of the GNU General Public
 * License, which you should have received with the source.
 *
 */
/*
 * Multiple Kernel Timers
 *
 * Make the period in the first lab long enough so you can issue
 * multiple writes before the timer function run. (Hint: you may want
 * to save your data before running this lab.)
 *
 * How many times does the function get run?
 *
 * Fix the solution so multiple timers work properly.
 @*/

#include <linux/module.h>
#include <linux/timer.h>
#include <linux/delay.h>

/* either of these (but not both) will work */
/* #include "lab_char.h" */
#include "lab_miscdev.h"

/* you probably don't need the ntimers variable, and it is a
   forward reference because we haven't done atomic variables
   but it is here to avoid unloading while there are still
   timers to unload.  It is also used sloppily on the exit :)
*/
static atomic_t ntimers;

struct my_dat {
	int l;
	struct timer_list *tl;
};

static void my_timer_function(unsigned long ptr)
{
	struct my_dat *tl = (struct my_dat *)ptr;
	dev_info(my_dev, "I am in my_timer_fun, jiffies = %ld\n", jiffies);
	dev_info(my_dev, " I think my current task pid is %d\n",
		 (int)current->pid);
	dev_info(my_dev, " my data is: %d\n", tl->l);
	kfree(tl->tl);
	kfree(tl);
	atomic_dec(&ntimers);
	dev_info(my_dev, "ntimers deced to %d\n", atomic_read(&ntimers));
}

static ssize_t
mycdrv_write(struct file *file, const char __user *buf, size_t lbuf,
	     loff_t *ppos)
{
	struct timer_list *tl;
	struct my_dat *mdata;
	static int len = 100;
	atomic_inc(&ntimers);
	dev_info(my_dev, "ntimers upped to %d\n", atomic_read(&ntimers));
	tl = kmalloc(sizeof(struct timer_list), GFP_KERNEL);
	dev_info(my_dev, " Entering the WRITE function\n");
	dev_info(my_dev, " my current task pid is %d\n", (int)current->pid);
	init_timer(tl);		/* intialize */
	tl->function = my_timer_function;
	tl->expires = jiffies + 4 * HZ;	/* four second delay */
	mdata = kmalloc(sizeof(struct my_dat), GFP_KERNEL);
	tl->data = (unsigned long)mdata;
	mdata->l = len;
	mdata->tl = tl;
	dev_info(my_dev, "Adding timer at jiffies = %ld\n", jiffies);
	add_timer(tl);
	len += 100;
	return mycdrv_generic_write(file, buf, lbuf, ppos);
}

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.read = mycdrv_generic_read,
	.write = mycdrv_write,
	.open = mycdrv_generic_open,
	.release = mycdrv_generic_release,
};

static int __init my_init(void)
{
	atomic_set(&ntimers, 0);
	return my_generic_init();
}

static void __exit my_exit(void)
{
	/* wait for all timers to finish ; pretty crummy */
	dev_info(my_dev, "ntimers in remove routine to %d\n",
		 atomic_read(&ntimers));
	while (atomic_read(&ntimers)) {
		dev_info(my_dev, "sleeping, ntimers still %d\n",
			 atomic_read(&ntimers));
		msleep(1000);	/* wait a second, ugly */
	}
	my_generic_exit();
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD331:4.2 s_10/lab2_multitimer.c");
MODULE_LICENSE("GPL v2");
