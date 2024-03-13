#include "print.h"
#include "init.h"
#include "debug.h"
#include "../lib/string.h"
#include "global.h"
#include "../lib/kernel/bitmap.h"
#include "memory.h"
#include "../thread/thread.h"

void k_thread_a(void*);

int main(void) {
   put_str("I am kernel\n");
   init_all();

   thread_start("k_thread_a", 31, k_thread_a, "argA ");

   while(1);
   return 0;
}

void k_thread_a(void* arg) {     
   char* para = arg;
   while(1) {
      int i = 9999999;
      while(i--);
      put_str(para);
   }
}