#include "memory.h"
#include "stdint.h"
#include "print.h"
#include "debug.h"

#define PG_SIZE 4096
#define MEM_BITMAP_BASE 0xc009a000
#define K_HEAP_START 0xc0100000

#define PDE_IDX(addr) ((addr & 0xffc00000) >> 22)
#define PTE_IDX(addr) ((addr & 0x003ff000) >> 12)

struct pool {
	struct bitmap pool_bitmap;
	uint32_t phy_addr_start;
	uint32_t pool_size;
};

struct pool kernel_pool, user_pool;
struct virtual_addr kernel_vaddr;

static void mem_pool_init(uint32_t all_mem) {
	put_str("	mem_pool_init start\n");
	uint32_t page_table_size = PG_SIZE * 256;

	uint32_t used_mem = page_table_size + 0x100000;
	uint32_t free_mem = all_mem - used_mem;
	uint16_t all_free_pages = free_mem / PG_SIZE;
	uint16_t kernel_free_pages = all_free_pages / 2;
	uint16_t user_free_pages = all_free_pages - kernel_free_pages;

	uint32_t kbm_length = kernel_free_pages / 8;
	uint32_t ubm_length = user_free_pages / 8;

	uint32_t kp_start = used_mem;
	uint32_t up_start = kp_start + kernel_free_pages * PG_SIZE;

	kernel_pool.phy_addr_start = kp_start;
	user_pool.phy_addr_start = up_start;

	kernel_pool.pool_size = kernel_free_pages * PG_SIZE;
	user_pool.pool_size = user_free_pages * PG_SIZE;

	kernel_pool.pool_bitmap.btmp_bytes_len = kbm_length;
	user_pool.pool_bitmap.btmp_bytes_len = ubm_length;

	kernel_pool.pool_bitmap.bits = (void*)MEM_BITMAP_BASE;

	user_pool.pool_bitmap.bits = (void*)(MEM_BITMAP_BASE + kbm_length);

	put_str("	kernel_pool_bitmap_start:");
	put_int((int)kernel_pool.pool_bitmap.bits);
	put_str("kernel_pool_phy_addr_start:");
	put_int(kernel_pool.phy_addr_start);
	put_str("\n");
	put_str("	user_pool_bitmap_start:");
	put_int((int)user_pool.pool_bitmap.bits);
	put_str("user_pool_phy_addr_start:");
	put_int(user_pool.phy_addr_start);
	put_str("\n");

	bitmap_init(&kernel_pool.pool_bitmap);
	bitmap_init(&user_pool.pool_bitmap);

	kernel_vaddr.vaddr_bitmap.btmp_bytes_len = kbm_length;
	kernel_vaddr.vaddr_bitmap.bits = (void*)(MEM_BITMAP_BASE + kbm_length + ubm_length);

	kernel_vaddr.vaddr_start = K_HEAP_START;
	bitmap_init(&kernel_vaddr.vaddr_bitmap);
	put_str("	mem_pool_init done\n");
}

void mem_init() {
	put_str("mem_init start\n");
	uint32_t mem_bytes_total = (*(uint32_t*)(0xb00));
	mem_pool_init(mem_bytes_total);
	put_str("mem_init done\n");
}

static void* vaddr_get(enum pool_flags pf, uint32_t pg_cnt) {
	int vaddr_start = 0;
	int bit_idx_start = -1;
	uint32_t cnt = 0;
	if(pf == PF_KERNEL) {
		bit_idx_start = bitmap_scan(&kernel_vaddr.vaddr_bitmap, pg_cnt);
		if(bit_idx_start == -1) {
			return NULL;
		}
		while(cnt < pg_cnt) {
			bitmap_set(&kernel_vaddr.vaddr_bitmap, bit_idx_start + cnt++, 1);
		}
		vaddr_start = kernel_vaddr.vaddr_start + bit_idx_start * PG_SIZE;
	}else {

	}
	return (void*)vaddr_start;
}

static void* palloc(struct pool* m_pool) {
	int bit_idx = bitmap_scan(&m_pool -> pool_bitmap, 1);
	if(bit_idx == -1) {
		return NULL;
	}
	bitmap_set(&m_pool -> pool_bitmap, bit_idx, 1);
	uint32_t page_phyaddr = ((bit_idx * PG_SIZE) + m_pool -> phy_addr_start);
	return (void*)page_phyaddr;
}

uint32_t* pde_ptr(uint32_t vaddr) {
	uint32_t pde = (uint32_t*)((0xfffff000) + PDE_IDX(vaddr) * 4);
	return pde;
}

uint32_t* pte_ptr(uint32_t vaddr) {
	uint32_t pte = (uint32_t*)(0xffc00000 + ((vaddr & 0xffc00000) >> 10) + PTE_IDX(vaddr) * 4);
	return pte;
}

static void page_table_add(void* _vaddr, void* _page_phyaddr) {
	uint32_t vaddr = (uint32_t)_vaddr, page_phyaddr = (uint32_t)_page_phyaddr;
	uint32_t* pde = pde_ptr(vaddr);
	uint32_t* pte = pte_ptr(vaddr);

	if(*pde & 0x00000001) {
		ASSERT(!(*pte & 0x00000001));

		if(!(*pte & 0x00000001)) {
			*pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
		}else {
			PANIC("pte repeat");
			// *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
		}
	}else {
		uint32_t pde_phyaddr = (uint32_t)palloc(&kernel_pool);

		*pde = (pde_phyaddr | PG_US_U | PG_RW_W | PG_P_1);

		memset((void*)((int)pte & 0xfffff000), 0, PG_SIZE);

		ASSERT(!(*pte & 0x00000001));
		*pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
	}
}

void* malloc_page(enum pool_flags pf, uint32_t pg_cnt) {
	ASSERT(pg_cnt > 0 && pg_cnt < 3840);
	void* vaddr_start = vaddr_get(pf, pg_cnt);
	if(vaddr_start == NULL) {
		return NULL;
	}

	uint32_t vaddr = (uint32_t)vaddr_start;
	uint32_t cnt = pg_cnt;
	struct pool* mem_pool = pf & PF_KERNEL ? &kernel_pool : &user_pool;

	while(cnt-- > 0) {
		void* page_phyaddr = palloc(mem_pool);
		if(page_phyaddr == NULL) {
			return NULL;
		}
		page_table_add((void*)vaddr, page_phyaddr);
		vaddr += PG_SIZE;
	}
	return vaddr_start;
}

void* get_kernel_pages(uint32_t pg_cnt) {
	void* vaddr = malloc_page(PF_KERNEL, pg_cnt);
	if(vaddr != NULL) {
		memset(vaddr, 0, pg_cnt * PG_SIZE);
	}
	return vaddr;
}