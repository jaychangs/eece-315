#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/slab.h>

struct birthday {
	int day;
	int month;
	int year;
	struct list_head list;
}birthday;

/* This function is called when the module is loaded. */
int simple_init(void)
{	
	struct birthday *person, *ptr, *next;
	static LIST_HEAD(birthday_list);
	int i;
	printk(KERN_INFO "Loading Module \n");

	for(i = 0; i < 5; i++){
		person = (struct birthday*) kmalloc(sizeof(*person), GFP_KERNEL);
		INIT_LIST_HEAD(&person->list);
		person->day = i+15;
		person->month = i+2;
		person->year = i+1990;
		list_add_tail(&person->list, &birthday_list);
	}
	list_for_each_entry_safe(ptr, next, &birthday_list, list){
		printk(KERN_INFO "Person %i is born on %i/%i/%i \n", i, ptr->day, 
			ptr->month, ptr->year);
	}
	return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void)
{
	printk(KERN_INFO "Removing Module \n");
}

/* Macros for registering module entry and exit points. */
module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");