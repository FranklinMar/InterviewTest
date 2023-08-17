#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO, KERN_ALERT */
#include <linux/init.h>   /* Needed for the macros */
#include <linux/stat.h>



#define DRIVER_AUTHOR "Denys Berezniuk"
#define DRIVER_DESC   "Watchpoint Test Kernel module"

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
	 
int __init initialize_driver(void)
{
	unsigned long long int *address = 0x12345;
	unsigned char value;
	printk(KERN_ALERT "TEST Driver loaded.\n");
	asm volatile("movb (%1), %0" : "=r" (value) : "r" (address));
	//printk(KERN_INFO "Pointer address: %p", pointer);
	//char first_byte = *((char*) pointer);
	//char first_byte = pointer[0];
	//pointer[0] = 1;
	//char second_byte = pointer[1];
	return 0;
}

void __exit driver_removal(void)
{
	printk(KERN_ALERT "TEST Driver removed.\n");
}

module_init(initialize_driver);
module_exit(driver_removal);

