#include "bitmap.h"
#include "stdint.h"     
#include "string.h"
#include "print.h"
#include "interrupt.h"
#include "debug.h"

void bitmap_init(struct bitmap* btmp) {
	memset(btmp->bits, 0, btmp->btmp_bytes_len);   
}

bool bitmap_scan_test(struct bitmap* btmp, uint32_t bit_idx) {
	uint32_t byte_idx = bit_idx / 8;
	uint32_t bit_odd  = bit_idx % 8;
	if(btmp -> bits[byte_idx] & (BITMAP_MASK << bit_odd)) {
		return 1;
	}
	else {
		return 0;
	}
}

int bitmap_scan(struct bitmap* bitmap, uint32_t cnt){
	uint32_t area_start = 0, area_size = 0;
	while(1){
		while(bitmap_scan_test(bitmap, area_start) && area_start / 8 < bitmap -> btmp_bytes_len) {
			area_start++;
		}
		
		if(area_start / 8 >= bitmap -> btmp_bytes_len) {
			return -1;
		}
		
		area_size = 1;
		
		while(area_size < cnt) {
			if((area_start + area_size) / 8 < bitmap->btmp_bytes_len){
				if(bitmap_scan_test(bitmap, area_start + area_size) == 0) {
					area_size++;
				}else {
					break;
				}
			}
			else {
				return -1;
			}
		}
		if(area_size == cnt)
			return area_start;
		area_start += (area_size + 1);
	}
}

void bitmap_set(struct bitmap* btmp, uint32_t bit_idx, int8_t value) {
	ASSERT((value == 0) || (value == 1));
	uint32_t byte_idx = bit_idx / 8;
	uint32_t bit_odd  = bit_idx % 8;

	if(value) {
		btmp->bits[byte_idx] |= (BITMAP_MASK << bit_odd);
	} else {
		btmp->bits[byte_idx] &= ~(BITMAP_MASK << bit_odd);
	}
}

