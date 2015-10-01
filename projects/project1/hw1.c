#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>

struct birthday {
  char *name;
  int day;
  int month;
  int year;
  struct list_head list;
};

static LIST_HEAD(birthday_list);

void add_birthday(char *name, int day, int month, int year){
  printk(KERN_INFO "adding %s to birthday_list...\n", name);
  struct birthday *birthday;
  birthday= kmalloc(sizeof(*birthday), GFP_KERNEL);
  birthday->name = name;
  birthday->day = day;
  birthday->month = month;
  birthday->year = year;
  INIT_LIST_HEAD(&birthday->list);
  list_add_tail(&birthday->list, &birthday_list);
  printk(KERN_INFO "added %s to birthday_list\n", name);
}
      
void print_birthday(struct birthday *birthday){
  printk(KERN_INFO "name: %s birthday: %d/%d/%d\n", birthday->name, birthday->month, birthday->day, birthday->year);
}

int hw1_init(void){
  printk(KERN_INFO "loading hw1 module...\n");

  add_birthday("aki", 19, 12, 1993);
  add_birthday("marvin", 2, 8, 1994);
  add_birthday("daniel", 29, 11, 1994);
  add_birthday("tony", 11, 7, 1994);
  add_birthday("yinjing", 9, 12, 1963);

  struct birthday *ptr;
  list_for_each_entry(ptr,&birthday_list, list){
    print_birthday(ptr);
  }
  
  printk(KERN_INFO "loaded module!\n");
  
  return 0;
}

void hw1_exit(void){
  printk(KERN_INFO "removing hw1 module...\n");
  struct birthday *ptr, *next;
  list_for_each_entry_safe(ptr, next, &birthday_list, list){
    list_del(&ptr->list);
    kfree(ptr);
  }
  printk(KERN_INFO "removed hw1 module!\n");
}

module_init(hw1_init);
module_exit(hw1_exit);
