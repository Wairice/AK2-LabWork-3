#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ktime.h>

MODULE_AUTHOR ("Stanislav Osipov <stasosipov09092004@gmail.com>");
MODULE_DESCRIPTION ("Hello, this is LabWork №3");
MODULE_LICENSE("Dual BSD/GPL");

static struct list_head hello_list_head;

struct hello_data {
    struct list_head list;
    ktime_t time;
};

static uint count = 1;
module_param(count, uint, S_IRUGO);
MODULE_PARM_DESC(count, "Count of times to print \"Hello, world!\"");

static int __init hello_init(void)
{
    int i;
    struct hello_data *ptr;

    if (count == 0) {
        pr_warn("Count is 0.\n");
    }
    
    if (count >= 5 && count <= 10) {
        pr_warn("Count is between 5 and 10.\n");
    }

    if (count > 10) {
        pr_err("Count is greater than 10. \nExiting with error.\n");
        return -EINVAL;
    }

    INIT_LIST_HEAD(&hello_list_head);

    for (i = 0; i < count; ++i) {
        ptr = kmalloc(sizeof(*ptr), GFP_KERNEL);
        if (!ptr)
            return -ENOMEM;

        ptr->time = ktime_get();
        list_add_tail(&ptr->list, &hello_list_head);

        pr_emerg("Hello, world!\n");
    }

    return 0;
}

static void __exit hello_exit(void)
{
    struct hello_data *ptr, *temp;

    list_for_each_entry_safe(ptr, temp, &hello_list_head, list) {
        pr_info("Time: %lld ns\n", ktime_to_ns(ptr->time));
        list_del(&ptr->list);
        kfree(ptr);
    }
}

module_init(hello_init);
module_exit(hello_exit);
