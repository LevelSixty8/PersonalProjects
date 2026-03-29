#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John Johnson");
MODULE_DESCRIPTION("CSE330 Project 2: parameterized module");

static char *charParameter = "Fall";
static int intParameter = 2025;
module_param(charParameter, charp, 0);
module_param(intParameter, int, 0);

static int __init my_module_init(void) {
	printk(KERN_INFO "Hello, I am John Johnson, a student of CSE330 %s %d.\n", charParameter, intParameter


);
	return 0;
}

static void __exit my_module_exit(void) {
	printk(KERN_INFO "Goodbye from John Johnson's kernel module.\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

