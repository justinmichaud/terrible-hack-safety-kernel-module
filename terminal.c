#include "terminal.h"

#include <linux/kernel.h>
#include <linux/module.h>

static void change_terminal_wq(struct work_struct * _arg) {
  char *argv[] = {"/usr/bin/bash", "-c", "chvt 3 && fbi /home/justinmichaud/terriblehack/bsod.png -a --noverbose --nointeractive -t 5 -1", NULL };
  char *envp[] = {"HOME=/", "TERM=linux", "PATH=/sbin:/usr/sbin:/bin:/usr/bin", NULL };
  
  printk("Change terminal wq starting\n");
  call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
  printk("Change terminal wq done\n");
}

DECLARE_WORK(jiq_work, change_terminal_wq);

void terrible_change_terminal() {
printk("Change terminal wq enqueing\n");
  schedule_work(&jiq_work);
}
