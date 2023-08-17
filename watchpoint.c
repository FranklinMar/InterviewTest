#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO, KERN_ALERT */
#include <linux/init.h>   /* Needed for the macros */
#include <linux/stat.h>
#include <linux/moduleparam.h> /* Needed for module_param */
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <linux/version.h>

#include <linux/string.h>
#include <linux/math.h>

//https://tldp.org/LDP/lkmpg/2.6/lkmpg.pdf
//https://stackoverflow.com/questions/19725900/watch-a-variable-memory-address-change-in-linux-kernel-and-print-stack-trace
//https://github.com/xcellerator/linux_kernel_hacking/issues/3


#define DRIVER_AUTHOR "Denys Berezniuk"
#define DRIVER_DESC   "Test Driver"

/*
 * On Linux kernels 5.7+, kallsyms_lookup_name() is no longer exported, 
 * so we have to use kprobes to get the address.
 * Full credit to @f0lg0 for the idea.
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
#define KPROBE_LOOKUP 1
#include <linux/kprobes.h>
static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"
};
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

struct perf_event * __percpu *write_watchpoint;
struct perf_event * __percpu *read_watchpoint;
char *address = NULL;
unsigned char *converted_address;
uint64_t pointer;
//uint64_t actual_address = 0;
uint64_t dec_address;
void *actual_address;
struct perf_event_attr write_attributes;
struct perf_event_attr read_attributes;
module_param(address, charp, /*0000*/ S_IRUGO /*| S_IWUGO*/);

MODULE_PARM_DESC(address, "Address to place a Watchpoint on");

static void write_handler(struct perf_event *bp, struct perf_sample_data *data, struct pt_regs *regs)
{
	printk(KERN_INFO "%s value is changed\n", address);
	dump_stack();
	printk(KERN_INFO "Dump stack from write_handler\n");
}

static void read_handler(struct perf_event *bp, struct perf_sample_data *data, struct pt_regs *regs)
{
	printk(KERN_INFO "%s value is read\n", address);
	dump_stack();
	printk(KERN_INFO "Dump stack from read_handler\n");
}
/*
bool starts_with(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}*/
	
int __init initialize_driver(void)
{
	#ifdef KPROBE_LOOKUP
		/*register_kprobe(&kp);
		kallsyms_lookup_name_custom = (unsigned long int (*)(const char *)) kp.addr;
		unregister_kprobe(&kp);*/
		typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
		kallsyms_lookup_name_t kallsyms_lookup_name;
		register_kprobe(&kp);
		kallsyms_lookup_name = (kallsyms_lookup_name_t) kp.addr;
		unregister_kprobe(&kp);
	#endif

	printk(KERN_ALERT "Driver loaded\nAddress is: '%s'\n", address);
	hw_breakpoint_init(&write_attributes);
	hw_breakpoint_init(&read_attributes);
	pointer = 0;
	//write_attributes.bp_addr = kallsyms_lookup_name(address);
	/*if(starts_with(address, "0x"))
	{
		dec_address = simple_strtoul(address + 2, NULL, 16);
	} else {
		dec_address = simple_strtoul(address, NULL, 16);
	}*/
	sscanf(address, "%llx", &dec_address);
	
	//actual_address = kzalloc(sizeof(uint64_t), GFP_KERNEL);
	printk(KERN_INFO "Address converted to decimal: %llu", dec_address);
	actual_address = (void *) 0x1;
	for (int i = 0; i < dec_address; i++)
	{
		actual_address++;
	}
	printk(KERN_INFO "Address calculated from NULL and he value: 0x%p", actual_address);
	
	//sscanf(address, "%llx", &actual_address);
	//sscanf(address, "%p", &actual_address);
	//for (volatile long long unsigned int i = 0; i < actual_address; i++, zero_address++);
	//zero_address += actual_address;
	//printk(KERN_INFO "Address converted = %p", zero_address);
	/*sscanf(address, "%p", &actual_address);
	sprintf(address, "%p", actual_address);
	printk(KERN_INFO "Address converted: %p", address);
	converted_address = kmalloc(sizeof(unsigned char) * 20, GFP_KERNEL);
	sprintf(converted_address, "%p", pointer);
	for (pointer = (void *) 0; strcmp(converted_address, address) <= 0; )
	{
		pointer++;
		sprintf(converted_address, "%p", pointer);
	}*/
	write_attributes.bp_addr = dec_address;//(uint64_t) actual_address;//(long long unsigned int) zero_address;
	write_attributes.bp_len = HW_BREAKPOINT_LEN_1;
	write_attributes.bp_type = HW_BREAKPOINT_W; //| HW_BREAKPOINT_R;
	write_watchpoint = register_wide_hw_breakpoint(&write_attributes, (perf_overflow_handler_t) write_handler, NULL);
	if(IS_ERR((void __force *) write_watchpoint))
	{
		int result = PTR_ERR((void __force *) write_watchpoint);
		printk(KERN_ALERT "Write Watchpoint registration failed\n");
		return result;
	}
	printk(KERN_INFO "HW Watchpoint (Breakpoint) for %s write installed (0x%p)\n", address, (void*)write_attributes.bp_addr);
	
	read_attributes.bp_addr = dec_address;//(uint64_t) actual_address;//(long long unsigned int) zero_address;
	read_attributes.bp_len = HW_BREAKPOINT_LEN_1;
	read_attributes.bp_type = HW_BREAKPOINT_W; //| HW_BREAKPOINT_R;
	read_watchpoint = register_wide_hw_breakpoint(&read_attributes, (perf_overflow_handler_t) read_handler, NULL);
	if(IS_ERR((void __force *) read_watchpoint))
	{
		int result = PTR_ERR((void __force *) read_watchpoint);
		printk(KERN_ALERT "Read Watchpoint registration failed\n");
		return result;
	}
	printk(KERN_INFO "HW Watchpoint (Breakpoint) for %s read installed (0x%p)\n", address, (void*)read_attributes.bp_addr);
	return 0;
}

void __exit driver_removal(void)
{
	printk(KERN_ALERT "Driver removed.\n");
}

module_init(initialize_driver);
module_exit(driver_removal);
