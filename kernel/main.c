#include "print.h"
#include "init.h"
#include "debug.h"
#include "../lib/string.h"
#include "global.h"
#include "../lib/kernel/bitmap.h"
#include "memory.h"

int main(void) {
   // int i;
   // struct bitmap btm;
   // struct bitmap* bitmap = &btm;
   // uint8_t test_map[2];
   // bitmap -> btmp_bytes_len = sizeof(test_map);
   // bitmap -> bits = test_map;
   // bitmap_init(bitmap);

   // for(i = 0; i < 4; i++){  
   //    bitmap_set(bitmap, i, 1);
   // }
   // bitmap_set(bitmap, 6, 1);
   // bitmap_set(bitmap, 7, 1);
   // bitmap_set(bitmap, 11, 1);
   // put_char('\n');
   // for(i = 0; i < 16; i++){
   //    put_int(bitmap_scan_test(bitmap, i));
   // }
   // put_char('\n');
   // put_int(bitmap_scan(bitmap, 4));
   // put_char('\n');
   // put_int(bitmap_scan(bitmap, 5));
   put_str("I am kernel\n");
   init_all();
   while(1);
   return 0;
}
