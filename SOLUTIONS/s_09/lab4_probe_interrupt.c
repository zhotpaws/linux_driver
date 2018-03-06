/* **************** LFD331:4.2 s_09/lab4_probe_interrupt.c **************** */
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
 * Using kprobes in an interrupt handler
 *
 * Adapt the previous exercise to work in a simple interrupt handler.
 * Is this completely legal?
 @*/

#include <linux/module.h>
#include <linux/kprobes.h>

static int my_interrupt_probe(struct inode *inode, struct file *file)
{
	pr_info("\n\n ****JPROBE**** in my_interrupt_debug\n\n");
	jprobe_return();
	return 0;
}

static struct kprobe kp;

static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	pr_info("pre_handler: p->addr=0x%p\n", p->addr);
	return 0;
}

static void handler_post(struct kprobe *p, struct pt_regs *regs,
			 unsigned long flags)
{
	pr_info("post_handler: p->addr=0x%p\n", p->addr);
}

static int handler_fault(struct kprobe *p, struct pt_regs *regs, int trapnr)
{
	pr_info("fault_handler:p->addr=0x%p\n", p->addr);
	return 0;
}

static struct jprobe jp = {
	.kp.symbol_name = "my",
	.entry = (kprobe_opcode_t *) my_interrupt_probe,
};

static int __init my_init(void)
{
	/* set the handler functions */

	kp.pre_handler = handler_pre;
	kp.post_handler = handler_post;
	kp.fault_handler = handler_fault;
	kp.symbol_name = "my_interrupt";

	if (register_kprobe(&kp)) {
		pr_info("Failed to register kprobe, quitting\n");
		return -1;
	}
	register_jprobe(&jp);
	pr_info("plant jprobe at %p, handler addr %p\n", jp.kp.addr, jp.entry);
	return 0;
}

static void __exit my_exit(void)
{

	unregister_jprobe(&jp);
	unregister_kprobe(&kp);
	pr_info("k,jprobes unregistered\n");
}

module_init(my_init);
module_exit(my_exit);
MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD331:4.2 s_09/lab4_probe_interrupt.c");
MODULE_LICENSE("GPL v2");
