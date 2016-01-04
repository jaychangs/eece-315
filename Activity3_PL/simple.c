#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/types.h>

struct birthday {
  int day;
  int month;
  int year;
  struct list_head list;
} birthday;

int simple_init (void)
{
  struct birthday *person, *ptr, *next;
  static LIST_HEAD(birthday_list);
  int i;
  printk(KERN_INFO "Loading Module\n");

  for (i=0; i<5; i++) {
    struct birthday *person;
    person = kmalloc(sizeof (*person), GFP_KERNEL);
    INIT_LIST_HEAD(&person->list);
    person->day = 2+i;
    person->month = 8+i;
    person->year = 1995+i;
    list_add_tail(&person->list, &birthday_list);
  }

  list_for_each_entry_safe(ptr, next, &birthday_list, list) {
    printk(KERN_INFO "Person's birthday is: %i/%i/%i\n", ptr->month, ptr->day, ptr->year);
  }

  return 0;

}

void simple_exit (void)
{
  printk(KERN_INFO "Removing Module\n");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");
