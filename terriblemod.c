#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include "terriblekbs.h"
#include "terminal.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Justin Michaud");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("Do terrible things to your kernel");

static void on_keyboard_update(char *buffer) {
  pr_debug("Keyboard update: %s\n", buffer);
  
  if (strncmp(buffer, "blockchain", 10) == 0) {
    terrible_change_terminal();
  } 
}

static int __init terrible_init(void)
{
  printk("Starting Terrible Things!\n");
  
	if (terrible_init_keyboard(&on_keyboard_update)) {
	  printk("Terrible Things failed to load: error initializing keyboard listener!\n");
	  return 0;
	}
	printk("Done Starting Terrible Things!\n");
	return 0;
}

static void __exit terrible_exit(void)
{
	terrible_uninit_keyboard();
	printk("Stopping Terrible Things!\n");
}

module_init(terrible_init);
module_exit(terrible_exit);
