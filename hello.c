
/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/slab.h>

MODULE_AUTHOR("Serhii Popovych <serhii.popovych@globallogic.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");
/*
 * Модифікувати модуль, додавши до нього
 * параметр типу uint, який визначає,
 * скільки разів має бути надрукований
 * рядок “Hello, world!”
 * A. Значення параметра за умовчанням 1.
 */
static uint count = 1;

/*
 * D. Додати опис параметра
 * (для його виводу командою modinfo).
 */
module_param(count, uint, S_IRUGO);
MODULE_PARM_DESC(count, "count of repeats hello world");

/*
 * III. Оголосити структуру даних
 * для розміщення у списку, яка крім
 * елемента struct list_head містить
 * поле типу ktime_t
 */
struct hello_data {
	struct list_head tlist;
	ktime_t time;
};

/*
 * IV. Створити статичну змінну голови списку.
 */
static struct list_head list;
static struct hello_data *data;

static int __init hello_init(void)
{
unsigned int i;

/*
 * B. Якщо значення параметра 0 або
 * знаходиться між 5 і 10, надрукувати
 * попередження і продовжити роботу.
 *
 * C. Якщо значення параметра більше 10,
 * то функція ініціалізації повинна
 * надрукувати повідомлення про помилку
 * і повернути значення -EINVAL
 * (модуль не має завантажити взагалі).
 */
if (count == 0) {
	printk(KERN_ALERT "The parameter value is equal to 0.\n");
} else if (count >= 5 && count <= 10) {
	printk(KERN_ALERT "The parameter value is between 5 and 10.\n");
} else if (count > 10) {
	printk(KERN_ALERT "Error. The parameter value is greater than 10.\n");
	return -EINVAL;
}

/*
 * V. Перед кожним друком привітання
 * виділити памʼять для екземпляра
 * оголошеної структури, занести в неї
 * поточний час ядра, отриманий функцією
 * ktime_get().
 */
INIT_LIST_HEAD(&list);

for (i = 0; i < count; i++) {
	struct hello_data *data = kmalloc(sizeof(*data), GFP_KERNEL);

	data->time = ktime_get();
	INIT_LIST_HEAD(&data->tlist);
	list_add_tail(&data->tlist, &list);

	printk(KERN_ALERT "Hello, world!\n");
}
return 0;
}

static void __exit hello_exit(void)
{
unsigned int i;

/*
 * VI. У функції hello_exit() пройти
 * по списку і надрукувати час кожної
 * події в наносекундах, вилучити елемент
 * списку і звільнити виділену памʼять.
 */
struct hello_data *tmp;
for (i = 0; i < count; i++) {
	list_for_each_entry_safe(data, tmp, &list, tlist) {
		s64 nanoseconds = ktime_to_ns(data->time);

		printk(KERN_ALERT "Time in nanoseconds: %lld\n", nanoseconds);
		list_del(&data->tlist);
		kfree(data);
	}
}
}

module_init(hello_init);
module_exit(hello_exit);
