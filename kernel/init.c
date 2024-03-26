#include "init.h"
#include "print.h"
#include "interrupt.h"
#include "/home/lxc_990131/桌面/MyOS/device/timer.h"
#include "memory.h"
#include "../thread/thread.h"
#include "../device/console.h"

void init_all() {
	put_str("init_all\n");
	idt_init();
	mem_init();
	thread_init();
	timer_init();
	console_init();
}
