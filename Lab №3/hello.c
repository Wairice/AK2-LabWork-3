#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ktime.h>

MODULE_AUTHOR("Stanislav Osipov <stasosipov09092004@gmail.com>");
MODULE_DESCRIPTION("Hello, this is LabWork №3");
MODULE_LICENSE("Dual BSD/GPL");

// Структура для подій, яка містить list_head і ktime_t
struct hello_event {
    struct list_head list;
    ktime_t time;
};

// Голова списку
static struct list_head hello_list_head;

// Параметр для визначення кількості разів друкування "Hello, world!"
static uint count = 1; // Параметр типу uint з 1 за замовчуванням
module_param(count, uint, S_IRUGO);
MODULE_PARM_DESC(count, "Count of times to print \"Hello, world!\"");

// Функція ініціалізації модуля
static int __init hello_init(void)
{
    int i;
    struct hello_event *ptr;

    // Перевірка на валідність значення count
    if (count == 0) {
        pr_warn("Count is 0.\n"); // Якщо значення параметра 0, друкується попередження, але робота продовжу'ється.
    }

    if (count >= 5 && count <= 10) {
        pr_warn("Count is between 5 and 10.\n"); // Якщо значення параметра знаходиться між 5 і 10, друкується попередження, але робота продовжу'ється.
    }

    if (count > 10) {
        pr_err("Count is greater than 10. \nExiting with error.\n"); // Якщо значення більше 10, друкується повідомлення про помилку і повернутається значення -EINVAL
        return -EINVAL;
    }

    INIT_LIST_HEAD(&hello_list_head); // Ініціалізація голови списку

    for (i = 0; i < count; ++i) {
        ptr = kmalloc(sizeof(*ptr), GFP_KERNEL); // Виділення пам'яті для структури
        if (!ptr)
            return -ENOMEM;

        ptr->time = ktime_get(); // Запис поточного часу в структуру
        list_add_tail(&ptr->list, &hello_list_head); // Додавання структури до списку

        pr_emerg("Hello, world!\n");
    }

    return 0;
}

// Функція видалення модуля
static void __exit hello_exit(void)
{
    struct hello_event *ptr, *temp;

    // Прохід по списку
    list_for_each_entry_safe(ptr, temp, &hello_list_head, list) {
        pr_info("Time: %lld ns\n", ktime_to_ns(ptr->time)); // Друк часу події в наносекундах
        list_del(&ptr->list); // Вилучення елемента зі списку
        kfree(ptr); // Звільнення пам'яті
    }
}

module_init(hello_init);
module_exit(hello_exit);
