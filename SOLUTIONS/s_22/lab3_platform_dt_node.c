/* **************** LFD331:4.2 s_22/lab3_platform_dt_node.c **************** */
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
 * Basic Platform Driver with FDT / DT support
 *
 * Write a very basic platform driver, containing only init() and
 * exit() functions, and dummy probe() and remove() functions.
 *
 * Using the driver you write or the skeleton one provided, load it
 * and check under /sys/bus/platform to make sure the driver is
 * showing up.
 *
 * ### Extend it to us DT ! ###
 *
@*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/of.h>   // of_match_ptr
#include <linux/of_device.h> //of_match_device

static struct of_device_id dummy_dt_ids[] = {
     { .compatible = "fake,dummy" },
     { }
};
MODULE_DEVICE_TABLE(of, dummy_dt_ids);


static int my_platform_probe(struct platform_device *pdev)
{
	int ret;
	const char *my_property;
	const struct of_device_id *of_id =
             of_match_device(dummy_dt_ids, &pdev->dev);

	if(!of_id){
	    pr_info("Failed to aquire of_id!\n");
	    return -ENODEV;
	}
	
	ret = of_property_read_string(pdev->dev.of_node, "myproperty", &my_property);
	if(ret < 0) {
	    pr_info("of_property_read_string failed!\n");
	    return ret;
	}

	pr_info("of_property: myproperty = %s", my_property);

	pr_info("Hit my_platform_probe2\n");
	return 0;
}

static int my_platform_remove(struct platform_device *pdev)
{
	pr_info("Hit my_platform_remove2\n");
	return 0;
}

static struct platform_driver my_platform_driver = {
	.driver = {
		   .name = "my_platform",
		   .owner = THIS_MODULE,
		   .of_match_table = of_match_ptr(dummy_dt_ids),
		   },
	.probe = my_platform_probe,
	.remove = my_platform_remove,
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

MODULE_AUTHOR("Jan-Simon Moeller");
MODULE_DESCRIPTION("LFD331:4.2 s_22/lab3_platform_dt_node.c");
MODULE_LICENSE("GPL v2");
