#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/sched.h>

void dfs_process(struct task_struct* head){

    struct task_struct *task;
    struct list_head *list;
    if(head == NULL) return;
    task = head;
    list_for_each(list, &(task->children)) {
        task = list_entry(list, struct task_struct, sibling);
        if(task->pid == 0) return;
        if(task->state == -1)
            printk(KERN_INFO "Name:%16s PID:%6i State:Unrunnable\n", task->comm, task->pid);
        else if(task->state == 0)
            printk(KERN_INFO "Name:%16s PID:%6i State:Runnable\n", task->comm, task->pid);
        else
            printk(KERN_INFO "Name:%16s PID:%6i State:Stopped\n", task->comm, task->pid);
        dfs_process(task);
    }
    return;
}

/* This function is called when the module is loaded. */
int simple_init(void){

    printk(KERN_INFO "Loading Module \n");
    dfs_process(&init_task);
/*  
    list_for_each(list, &(init_task.children)) {
		task = list_entry(list, struct task_struct, sibling);
		if(task->state == -1)
            printk(KERN_INFO "Name:%16s PID:%6i State:Unrunnable\n", task->comm, task->pid);
        else if(task->state == 0)
            printk(KERN_INFO "Name:%16s PID:%6i State:Runnable\n", task->comm, task->pid);
        else
            printk(KERN_INFO "Name:%16s PID:%6i State:Stopped\n", task->comm, task->pid);
	}
*/
/*
    for (list = init_task.children->next; prefetch(list->next), list != (init_task.children->next); list = list->next){
    	task = list_entry(list, struct task_struct, sibling);
    	printk(KERN_INFO "%i", task->pid);
    	if(task->state == -1)
            printk(KERN_INFO "Name:%16s PID:%6i State:Unrunnable\n", task->comm, task->pid);
        else if(task->state == 0)
            printk(KERN_INFO "Name:%16s PID:%6i State:Runnable\n", task->comm, task->pid);
        else
            printk(KERN_INFO "Name:%16s PID:%6i State:Stopped\n", task->comm, task->pid);
    }
*/

/*
    for_each_process(task){
        if(task->state == -1)
            printk(KERN_INFO "Name:%16s PID:%6i State:Unrunnable\n", task->comm, task->pid);
        else if(task->state == 0)
            printk(KERN_INFO "Name:%16s PID:%6i State:Runnable\n", task->comm, task->pid);
        else
            printk(KERN_INFO "Name:%16s PID:%6i State:Stopped\n", task->comm, task->pid);
    }
*/
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