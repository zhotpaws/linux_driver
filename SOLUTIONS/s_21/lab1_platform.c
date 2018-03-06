/* **************** LFD331:4.2 s_21/lab1_platform.c **************** */
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
 * Basic Platform Driver Skeleton
 *
 * Write a very basic platform driver, containing only init() and
 * exit() functions, and dummy probe() and remove() functions.
 *
 * Using the driver you write or the skeleton one provided, load it
 * and check under /sys/bus/platform to make sure the driver is
 * showing up.
 *
@*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>

static int my_platform_probe(struct platform_device *pdev)
{
	pr_info("Hit my_platform_probe\n");
	return 0;
}

static int my_platform_remove(struct platform_device *pdev)
{
	pr_info("Hit my_platform_remove\n");
	return 0;
}

static const struct platform_device_id my_platform_id_table[] = {
	{"my_platform_1", 0},
	{"my_platform_2", 0},
	{},
};

MODULE_DEVICE_TABLE(platform, my_platform_id_table);

static struct platform_driver my_platform_driver = {
	.driver = {
		   .name = "my_platform",
		   .owner = THIS_MODULE,
		   },
	.probe = my_platform_probe,
	.remove = my_platform_remove,
	.id_table = my_platform_id_table,
};

static int __init my_init(void)
{
	pr_info("Loading my platform device\n");
	return platform_driver_register(&my_platform_driver);
}

static void __exit my_exit(void)
{
	pr_info("Unloading my platform device\n");
	platform_driver_unregister(&my_platform_driver);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD331:4.2 s_21/lab1_platform.c");
MODULE_LICENSE("GPL v2");
