#include "init.h"
#include "print.h"
#include "interrupt.h"
#include "/home/lxc_990131/桌面/MyOS/device/timer.h"
#include "memory.h"

void init_all() {
	put_str("init_all\n");
	idt_init();
	timer_init();
	mem_init();
}
